// Non-branching conditionals

// When equal to
float4 when_eq(float4 x, float4 y)
{
	return 1.0 - abs(sign(x - y));
}

// When not equal to
float4 when_neq(float4 x, float4 y)
{
	return abs(sign(x - y));
}

// When greater than
float4 when_gt(float4 x, float4 y)
{
	return max(sign(x - y), 0.0);
}

// When less than
float4 when_lt(float4 x, float4 y)
{
	return max(sign(y - x), 0.0);
}

// When greater than or equal to
float4 when_ge(float4 x, float4 y)
{
	return 1.0 - when_lt(x, y);
}

// When less than or equal to
float4 when_le(float4 x, float4 y)
{
	return 1.0 - when_gt(x, y);
}

// Non-branching logical operators
float4 and(float4 a, float4 b)
{
	return a * b;
}

float4 or(float4 a, float4 b)
{
	return min(a + b, 1.0);
}

float4 xor(float4 a, float4 b)
{
	return (a + b) % 2.0;
}

float4 not(float4 a)
{
	return 1.0 - a;
}

// Remap to range
//		Remaps the given input value from the range low1-high1 to the range low2-high2
float remapToRange(float value, float low1, float high1, float low2, float high2)
{
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
	//return low2 + (high2 - low2) * ((value - low1) / (high1 - low1));
}

float3 AdjustContrast(float3 color, float contrast)
{
	return saturate(lerp(float3(0.5, 0.5, 0.5), color, contrast));
}

float3 AdjustContrastCurve(float3 color, float contrast)
{
	return pow(abs(color * 2 - 1), 1 / max(contrast, 0.0001)) * sign(color - 0.5) + 0.5;
}