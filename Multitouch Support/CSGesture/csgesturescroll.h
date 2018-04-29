// CSGesture Multitouch Touchpad Library
// © 2016, CoolStar. All Rights Reserved.

#include <IOKit/IOService.h>
#include "AverageClasses.h"
#include "VoodooI2CCSGestureEngine.hpp"

#ifndef csgesturescroll_h
#define csgesturescroll_h

class VoodooCSGestureHIPointingWrapper;

class CSGestureScroll : public IOService {
    typedef IOService super;
    OSDeclareDefaultStructors(CSGestureScroll);
    
private:
    SimpleAverage<int, 32> dy_history;
    SimpleAverage<int, 32> dx_history;
    
    int lastx1 = 0;
    int lasty1 = 0;
    int lastx2 = 0;
    int lasty2 = 0;
    
    bool isTouchActive = false;
    
    int momentumscrollsamplesmin = 1;

    // @pqml fork change
    // Add a "float precision" variable
    // Add a frequency rate for the scroll timer
    int fprecision = 100000;
    int timerfreq = 4;

    // @pqml fork change
    // Only use one rest variable per dimension
    int momentumscrollcurrentx = 0;
    int momentumscrollmultiplierx = 98;
    int momentumscrolldivisorx = 100;
    int momentumscrollrestx = 0;
    
    int momentumscrollcurrenty = 0;
    int momentumscrollmultipliery = 98;
    int momentumscrolldivisory = 100;
    int momentumscrollresty = 0;
    
    bool cancelDelayScroll = false;
    
    int noScrollCounter = 0;
    
    IOTimerEventSource *_disableScrollDelayTimer;
    
    IOTimerEventSource *_scrollTimer;
    
    IOWorkLoop *_workLoop;
    
    void disableScrollDelayed();
    
    void disableScrollingDelayLaunch();
    
    void scrollTimer();
    
public:
    bool inertiaScroll = true;
    
    VoodooI2CCSGestureEngine *_gestureEngine;
    
    csgesture_softc *softc;
    
    VoodooCSGestureHIPointingWrapper *_pointingWrapper;
    
    void prepareToSleep();
    void wakeFromSleep();
    
    bool isScrolling();
    
    void stopScroll();
    
    bool start(IOService *provider) override;
    
    void stop();
    
    void ProcessScroll(int x1, int y1, int x2, int y2);
};
#endif /* csgesturescroll_h */
