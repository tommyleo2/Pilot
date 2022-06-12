#include "runtime/function/animation/animation_FSM.h"
#include <iostream>
namespace Pilot
{
    AnimationFSM::AnimationFSM() {}
    float tryGetFloat(const json11::Json::object& json, const std::string& key, float default_value)
    {
        auto found_iter = json.find(key);
        if (found_iter != json.end() && found_iter->second.is_number())
        {
            return found_iter->second.number_value();
        }
        return default_value;
    }
    bool tryGetBool(const json11::Json::object& json, const std::string& key, float default_value)
    {
        auto found_iter = json.find(key);
        if (found_iter != json.end() && found_iter->second.is_bool())
        {
            return found_iter->second.bool_value();
        }
        return default_value;
    }
    bool AnimationFSM::update(const json11::Json::object& signals)
    {
        States last_state     = m_state;
        bool   is_clip_finish = tryGetBool(signals, "clip_finish", false);
        bool   is_jumping     = tryGetBool(signals, "jumping", false);
        float  speed          = tryGetFloat(signals, "speed", 0);
        bool   is_moving      = speed > 0.01f;
        bool   start_walk_end = false;

        switch (m_state)
        {
            case States::_idle:
            {
                if (is_jumping)
                {
                    m_state = States::_jump_start_from_idle;
                }
                else if (is_moving)
                {
                    m_state = States::_walk_start;
                }
                break;
            }
            case States::_walk_start:
            {
                if (is_clip_finish)
                {
                    m_state = States::_walk_run;
                }
                break;
            }
            case States::_walk_run:
            {
                if (is_jumping)
                {
                    m_state = States::_jump_start_from_walk_run;
                }
                else if (start_walk_end && is_clip_finish)
                {
                    m_state = States::_walk_stop;
                }
                else if (!is_moving)
                {
                    m_state = States::_idle;
                }
                break;
            }
            case States::_walk_stop:
            {
                if (!is_moving && is_clip_finish)
                {
                    m_state = States::_idle;
                }
                break;
            }
            case States::_jump_start_from_idle:
            {
                if (is_clip_finish)
                {
                    m_state = States::_jump_loop_from_idle;
                }
                break;
            }
            case States::_jump_loop_from_idle:
            {
                if (!is_jumping)
                {
                    m_state = States::_jump_end_from_idle;
                }
                break;
            }
            case States::_jump_end_from_idle:
            {
                if (is_clip_finish)
                {
                    m_state = States::_idle;
                }
                break;
            }
            case States::_jump_start_from_walk_run:
            {
                if (is_clip_finish)
                {
                    m_state = States::_jump_loop_from_walk_run;
                }
                break;
            }
            case States::_jump_loop_from_walk_run:
            {
                if (!is_jumping)
                {
                    m_state = States::_jump_end_from_walk_run;
                }
                break;
            }
            case States::_jump_end_from_walk_run:
            {
                if (is_clip_finish)
                {
                    m_state = States::_walk_run;
                }
                break;
            }
            default:
                break;
        }
        return last_state != m_state;
    }

    std::string AnimationFSM::getCurrentClipBaseName() const
    {
        static std::unordered_map<States, std::string> state_clip_names{
            {States::_idle, "idle_walk_run"},
            {States::_walk_start, "walk_start"},
            {States::_walk_run, "idle_walk_run"},
            {States::_walk_stop, "walk_stop"},
            {States::_jump_start_from_walk_run, "jump_start"},
            {States::_jump_start_from_idle, "jump_start"},
            {States::_jump_loop_from_walk_run, "jump_loop"},
            {States::_jump_loop_from_idle, "jump_loop"},
            {States::_jump_end_from_walk_run, "jump_stop"},
            {States::_jump_end_from_idle, "jump_stop"},
        };
        auto it = state_clip_names.find(m_state);
        if (it != state_clip_names.end())
        {
            return it->second;
        }
        return "idle_walk_run";
//        switch (m_state)
//        {
//            case States::_idle:
//                return "idle_walk_run";
//            case States::_walk_start:
//                return "walk_start";
//            case States::_walk_run:
//                return "idle_walk_run";
//            case States::_walk_stop:
//                return "walk_stop";
//            case States::_jump_start_from_walk_run:
//            case States::_jump_start_from_idle:
//                return "jump_start";
//            case States::_jump_loop_from_walk_run:
//            case States::_jump_loop_from_idle:
//                return "jump_loop";
//            case States::_jump_end_from_walk_run:
//            case States::_jump_end_from_idle:
//                return "jump_stop";
//            default:
//                return "idle_walk_run";
//        }
    }
}

