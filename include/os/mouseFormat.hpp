#pragma once
#include <string>

enum class mouseButtons
{
    left,
    right,
    middle,
    side,
    extra,
    forward,
    back,
    task
};

enum class mouseAxis
{
    horizontal, 
    vertical
};

enum class mouseAxisSource
{
    wheel,
    finger,
    continuous,
    wheelTilt
};

static std::string mouseButtonToString(mouseButtons value)
{
    switch (value)
    {
    
        case mouseButtons::left:
            return "left";
            break;
        case mouseButtons::right:
            return "right";
            break;
        case mouseButtons::middle:
            return "middle";
            break;
        case mouseButtons::side:
            return "side";
            break;
        case mouseButtons::extra:
            return "extra";
            break;
        case mouseButtons::forward:
            return "forward";
            break;
        case mouseButtons::back:
            return "back";
            break;
        case mouseButtons::task:
            return "task";
            break;
    }
    return "no button of value";
}

static std::string mouseAxisToString(mouseAxis value)
{
    switch (value)
    {
        case mouseAxis::horizontal:
            return "horizontal";
            break;
        case mouseAxis::vertical:
            return "vertical";
            break;
    }
    return "no axis of value";

}


static std::string mouseAxisSourceToString(mouseAxisSource value)
{
    switch (value)
    {
        case mouseAxisSource::wheel:
            return "wheel";
            break;
        case mouseAxisSource::finger:
            return "finger";
            break;
        case mouseAxisSource::continuous:
            return "continuous";
            break;
        case mouseAxisSource::wheelTilt:
            return "wheelTilt";
            break;
    }
    return "no axis source of value";

}