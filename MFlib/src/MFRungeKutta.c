/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

MFRungeKutta.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>

static float _MFRungeKuttaTest (float t, float dt,float (*deltaFunc) (float,float),float *y) {
	float a2  =     1.0 /      5.0;
	float a3  =     3.0 /     10.0;
	float a4  =     3.0 /      5.0;
	float a5  =     1.0;
	float a6  =     7.0 /      8.0;
	float b21 =     1.0 /      5.0;
	float b31 =     3.0 /     40.0;
	float b41 =     3.0 /     10.0;
	float b51 =   -11.0 /     54.0;
	float b61 =  1631.0 /  55296.0;
	float b32 =     9.0 /     40.0;
	float b42 =    -9.0 /     10.0;
	float b52 =     5.0 /      2.0;
	float b62 =   175.0 /    512.0;
	float b43 =     6.0 /      5.0;
	float b53 =   -70.0 /     27.0;
	float b63 =   575.0 /  13824.0;
	float b54 =    35.0 /     27.0;
	float b64 = 44275.0 / 110592.0;
	float b65 =   253.0 /   4096.0;
	float c1  =    37.0 /    378.0;
	float c2  =     0.0;
	float c3  =   250.0 /    621.0;
	float c4  =   125.0 /    594.0;
	float c5  =     0.0;
	float c6  =   512.0 /   1771.0;
	float cs1 =  2825.0 /  27648.0;
	float cs2 =     0.0;
	float cs3 = 18575.0 /  48384.0;
	float cs4 = 13525.0 /  55296.0;
	float cs5 =   277.0 /  14336.0;
	float cs6 =     1.0 /      4.0;
	float ak2, ak3, ak4, ak5, ak6, yTemp, dydt;
	
	dydt = (*deltaFunc) (t,*y);
	yTemp = *y + b21 * dt * dydt;
	ak2 =  (*deltaFunc) (t + a2 * dt,yTemp);
	yTemp = *y + dt * (b31 * dydt + b32 * ak2);
	ak3 =  (*deltaFunc) (t + a3 * dt,yTemp);
	yTemp = *y + dt * (b41 *dydt + b42 * ak2 + b43 * ak3);
	ak4 =  (*deltaFunc) (t + a4 * dt,yTemp);
	yTemp = *y + dt * (b51 * dydt + b52 * ak2 + b53 * ak3 + b54 * ak4);
	ak5 =  (*deltaFunc) (t + a5 * dt,yTemp);
	yTemp = *y + dt * (b61 * dydt + b62 * ak2 + b63 * ak3 + b64 * ak4 + b65 * ak5);
	ak6 =  (*deltaFunc) (t + a6 * dt,yTemp);
	*y += dt * (c1 * dydt + c2 * ak2 + c3 * ak3 + c4 * ak4 + c5 * ak5 + c6 * ak6);
	return (dt * ((cs1 - c1) * dydt + (cs2 - c2) * ak2 + (cs3 - c3) * ak3 + (cs4 - c4) * ak4 + (cs5 - c5) * ak5 + (cs6 - c6) * ak6));
}

float MFRungeKutta (float t,float tStep,float y,float (*deltaFunc) (float,float)) {
	float time, dt, dtMin;
	float yTest, error, dyAbs;
	
	dt = tStep;
	dtMin = dt * MFTolerance / 10.0;
	for (time = t;time < t + tStep;) {
		while (dt > dtMin) {
			yTest = y;
			error = fabs (_MFRungeKuttaTest (t + time, dt,deltaFunc,&yTest));
			dyAbs = fabs (y - yTest);
			if (dyAbs < MFPrecision)
				{ if (error < MFTolerance * MFTolerance)	break; }
			else 
				{ if (error < MFTolerance * dyAbs)	break; }
			dt *= 0.500;
		}
		y = yTest;
		time += dt;
		dt *= 2.0;
		if (dt > t + tStep - time) dt = t + tStep - time;
	}
	return (y);    
}
