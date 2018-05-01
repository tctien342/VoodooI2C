// CSGesture Multitouch Touchpad Library
// © 2016, CoolStar. All Rights Reserved.

#include "csgesturescroll.h"
#include <stdint.h>
#include "VoodooCSGestureHIPointingWrapper.hpp"
#include <IOKit/IOTimerEventSource.h>

OSDefineMetaClassAndStructors(CSGestureScroll, IOService);

#ifndef ABS32
#define ABS32
inline int32_t abs(int32_t num){
    if (num < 0){
        return num * -1;
    }
    return num;
}
#endif

inline bool isSameSign(int n1, int n2) {
    if (n1 == 0 || n2 == 0)
        return true;
    if (n1 > 0 && n2 > 0)
        return true;
    if (n1 < 0 && n2 < 0)
        return true;
    return false;
}

inline bool isPropertyValid(int prop) {
    if (prop == 65535)
        return false;
    return true;
}

void CSGestureScroll::disableScrollDelayed(){
    if (!cancelDelayScroll){
        softc->scrollInertiaActive = false;
        cancelDelayScroll = false;
    }
    
    if (_disableScrollDelayTimer){
        _disableScrollDelayTimer->cancelTimeout();
        _disableScrollDelayTimer->release();
        _disableScrollDelayTimer = NULL;
    }
}

void CSGestureScroll::disableScrollingDelayLaunch(){
    if (softc->scrollInertiaActive)
        return;
    
    cancelDelayScroll = false;
    
    if (_disableScrollDelayTimer){
        _disableScrollDelayTimer->cancelTimeout();
        _disableScrollDelayTimer->release();
        _disableScrollDelayTimer = NULL;
    }
    
    _disableScrollDelayTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &CSGestureScroll::disableScrollDelayed));
    _workLoop->addEventSource(_disableScrollDelayTimer);
    _disableScrollDelayTimer->setTimeoutMS(300);
}

bool CSGestureScroll::isScrolling(){
    if (isTouchActive)
        return true;
    
    // @pqml fork change
    // Only use one rest variable per dimension
    if (momentumscrollcurrenty > 0 || momentumscrollresty > 0)
        return true;
    
    if (momentumscrollcurrentx > 0 || momentumscrollrestx > 0)
        return true;
    
    return false;
}

void CSGestureScroll::stopScroll(){
    // @pqml fork change
    // Only use one rest variable per dimension
    momentumscrollcurrentx = 0;
    momentumscrollrestx = 0;
    dx_history.reset();
    
    momentumscrollcurrenty = 0;
    momentumscrollresty = 0;
    dy_history.reset();
    isTouchActive = false;
}

void CSGestureScroll::scrollTimer(){
    // @pqml fork change
    // Change the way the momentum is computed for x
    if (momentumscrollcurrentx) {
        int dx = 0;
        if (abs(momentumscrollcurrentx) >= fprecision) {
            dx += momentumscrollcurrentx / fprecision;
        }
        if (abs(momentumscrollrestx) >= fprecision) {
            dx += momentumscrollrestx / fprecision;
            momentumscrollrestx = momentumscrollrestx % fprecision;
        }
        
        if (abs(momentumscrollcurrentx) > 0) {
            if (inertiaScroll && abs(dx) > 0) _pointingWrapper->updateScroll(0, dx, 0);
            momentumscrollcurrentx *= 0.983;
            momentumscrollrestx += momentumscrollcurrentx % fprecision;
        } else {
            momentumscrollcurrentx = 0;
            momentumscrollrestx = 0;
        }
    }
    
    // @pqml fork change
    // Change the way the momentum is computed for y
    if (momentumscrollcurrenty) {
        int dy = 0;
        if (abs(momentumscrollcurrenty) >= fprecision) {
            dy += momentumscrollcurrenty / fprecision;
        }
        if (abs(momentumscrollresty) >= fprecision) {
            dy += momentumscrollresty / fprecision;
            momentumscrollresty = momentumscrollresty % fprecision;
        }
        
        if (abs(momentumscrollcurrenty) > 0) {
            if (inertiaScroll && abs(dy) > 0) _pointingWrapper->updateScroll(dy, 0, 0);
            // 0.9775
            momentumscrollcurrenty *= 0.983;
            momentumscrollresty += momentumscrollcurrenty % fprecision;
        } else {
            momentumscrollcurrenty = 0;
            momentumscrollresty = 0;
        }
    }
    
    if (momentumscrollcurrentx != 0 || momentumscrollcurrenty != 0) {
        cancelDelayScroll = true;
        softc->scrollInertiaActive = true;
    }

    // @pqml fork change
    // Change scrolltimer frequency
    _scrollTimer->setTimeoutMS(timerfreq);
}

void CSGestureScroll::ProcessScroll(int x1, int y1, int x2, int y2) {
    if (isPropertyValid(x1) &&
        isPropertyValid(y1) &&
        isPropertyValid(x2) &&
        isPropertyValid(y2)){
        
        int delta_x1 = x1 - lastx1;
        int delta_y1 = y1 - lasty1;
        
        int delta_x2 = x2 - lastx2;
        int delta_y2 = y2 - lasty2;
        
        bool ignoreScroll = false;
        
        if (lastx1 == 0 || lasty1 == 0 || lastx2 == 0 || lasty2 == 0)
            ignoreScroll = true;
            
        lastx1 = x1;
        lasty1 = y1;
        lastx2 = x2;
        lasty2 = y2;
        
        if (ignoreScroll)
            return;
        
        int avgy = (delta_y1 + delta_y2) / 2;
        int avgx = (delta_x1 + delta_x2) / 2;
        
        if (abs(avgx) > 100)
            avgx = 0;
        if (abs(avgy) > 100)
            avgx = 0;
        
        if (avgx == 0 && avgy == 0) {
            noScrollCounter++;

            // @pqml fork change
            // Always reset the momentum when touching the trackpad with two fingers
            if (isTouchActive){
                momentumscrollcurrenty = 0;
                momentumscrollresty = 0;
                momentumscrollcurrentx = 0;
                momentumscrollrestx = 0;
            }

            if (noScrollCounter < 3)
                return;
        }
        else {
            noScrollCounter = 0;
        }
        
        avgy = -avgy;
        avgx = -avgx;
        
        if (abs(avgy) > abs(avgx)) {
            _pointingWrapper->updateScroll(avgy, 0, 0);
            
            // @pqml fork change
            // Only use one rest variable per dimension
            // Always reset the momentum when touching the trackpad with two fingers
            momentumscrollcurrenty = 0;
            momentumscrollresty = 0;
            if (!isSameSign(momentumscrollcurrenty, avgy)) dy_history.reset();
            
            dy_history.filter(avgy);
            dx_history.reset();
        } else {
            _pointingWrapper->updateScroll(0, avgx, 0);
            
            // @pqml fork change
            // Only use one rest variable per dimension
            // Always reset the momentum when touching the trackpad with two fingers
            momentumscrollcurrentx = 0;
            momentumscrollrestx = 0;
            if (!isSameSign(momentumscrollcurrentx, avgx)) dx_history.reset();
            
            dx_history.filter(avgx);
            dy_history.reset();
        }
        ;
        isTouchActive = true;
    } else {
        if (isTouchActive){
            // @pqml fork change
            // Change the way the momentum is computed for x and y
            if (dx_history.count() > momentumscrollsamplesmin) {
                momentumscrollcurrenty = dx_history.average() * fprecision;
                momentumscrollrestx = 0;
            }
            if (dy_history.count() > momentumscrollsamplesmin) {
                momentumscrollcurrenty = dy_history.average() * fprecision;
                momentumscrollrestx = 0;
            }
            
            dx_history.reset();
            dy_history.reset();
            isTouchActive = false;
        }
        
        lastx1 = 0;
        lasty1 = 0;
        lastx2 = 0;
        lasty2 = 0;
    }
}

void CSGestureScroll::prepareToSleep(){
    if (_scrollTimer){
        _scrollTimer->cancelTimeout();
        _scrollTimer->release();
        _scrollTimer = NULL;
    }
    
    if (_disableScrollDelayTimer){
        _disableScrollDelayTimer->cancelTimeout();
        _disableScrollDelayTimer->release();
        _disableScrollDelayTimer = NULL;
    }
}

void CSGestureScroll::wakeFromSleep(){
    if (_scrollTimer){
        _scrollTimer->cancelTimeout();
        _scrollTimer->release();
        _scrollTimer = NULL;
    }
    
    _scrollTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &CSGestureScroll::scrollTimer));
    _workLoop->addEventSource(_scrollTimer);
    // @pqml fork change
    // Change scrolltimer frequency
    _scrollTimer->setTimeoutMS(timerfreq);
}

bool CSGestureScroll::start(IOService *provider){
    if (!super::start(provider))
        return false;
    
    _workLoop = getWorkLoop();
    if (!_workLoop){
        IOLog("CSGestureScroll: Failed to get workloop!\n");
        return false;
    }
    
    _workLoop->retain();
    
    _scrollTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &CSGestureScroll::scrollTimer));
    _workLoop->addEventSource(_scrollTimer);
    // @pqml fork change
    // Change scrolltimer frequency
    _scrollTimer->setTimeoutMS(timerfreq);
    if (_scrollTimer)
        return true;
    return false;
}

void CSGestureScroll::stop(){
    if (_scrollTimer){
        _scrollTimer->cancelTimeout();
        _scrollTimer->release();
        _scrollTimer = NULL;
    }
    
    if (_disableScrollDelayTimer){
        _disableScrollDelayTimer->cancelTimeout();
        _disableScrollDelayTimer->release();
        _disableScrollDelayTimer = NULL;
    }
    
    if (_workLoop) {
        _workLoop->release();
        _workLoop = NULL;
    }
}
