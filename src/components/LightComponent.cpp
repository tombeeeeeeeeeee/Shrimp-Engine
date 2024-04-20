#include "LightComponent.h"

void LightComponent::CalculateLinearQuadConstants()
{
	if (range <= 7)
	{
		linear = 1 - range * 0.3 / 7;
		quad = 1.8 + (7 - range) * 0.5;
	}
	else if (range <= 13)
	{
		float ratio = range - (13 - 7);
		ratio /= 7;
		linear = 0.7 + (0.35 - 0.7) * ratio;
		quad = 1.8 + (0.44 - 1.8) * ratio;
	}
	else if (range <= 20)
	{
		float ratio = range - (20 - 13);
		ratio /= 13;
		linear = 0.35 + (0.22 - 0.35) * ratio;
		quad = 0.44 + (0.2 - 0.44) * ratio;
	}
	else if (range <= 32)
	{
		float ratio = range - (32 - 20);
		ratio /= 20;
		linear = 0.22 + (0.14 - 0.22) * ratio;
		quad = 0.2 + (0.07 - 0.2) * ratio;
	}
	else if (range <= 50)
	{
		float ratio = range - (50 - 32);
		ratio /= 32;
		linear = 0.14 + (0.09 - 0.14) * ratio;
		quad = 0.07 + (0.032 - 0.07) * ratio;
	}
	else if (range <= 65)
	{
		float ratio = range - (65 - 50);
		ratio /= 50;
		linear = 0.09 + (0.07 - 0.09) * ratio;
		quad = 0.032 + (0.017 - 0.032) * ratio;
	}
	else if (range <= 100)
	{
		float ratio = range - (100 - 65);
		ratio /= 65;
		linear = 0.07 + (0.045 - 0.07) * ratio;
		quad = 0.017 + (0.0075 - 0.017) * ratio;
	}
	else if (range <= 160)
	{
		float ratio = range - (160 - 100);
		ratio /= 100;
		linear = 0.045 + (0.027 - 0.045) * ratio;
		quad = 0.0075 + (0.0028 - 0.0075) * ratio;
	}
	else if (range <= 200)
	{
		float ratio = range - (200 - 160);
		ratio /= 160;
		linear = 0.027 + (0.022 - 0.027) * ratio;
		quad = 0.0028 + (0.0019 - 0.0028) * ratio;
	}
	else if (range <= 325)
	{
		float ratio = range - (325 - 200);
		ratio /= 200;
		linear = 0.022 + (0.014 - 0.022) * ratio;
		quad = 0.0019 + (0.0007 - 0.0019) * ratio;
	}
	else if (range <= 600)
	{
		float ratio = range - (600 - 325);
		ratio /= 325;
		linear = 0.014 + (0.007 - 0.014) * ratio;
		quad = 0.0007 + (0.0002 - 0.0007) * ratio;
	}
	else
	{
		float ratio = range - (3250 - 600);
		ratio /= 600;
		linear = 0.007 + (0.0014 - 0.007) * ratio;
		quad = 0.0002 + (0.000007 - 0.0002) * ratio;
	}
}
