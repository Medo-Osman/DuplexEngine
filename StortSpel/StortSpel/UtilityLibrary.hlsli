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

float3 BlendAngleCorrectedNormals(float3 baseNormal, float3 AdditionalNormal)
{
	float2 rgMask1 = baseNormal.rg;
	float bMask1 = baseNormal.b;
	
	bMask1 += 1;
	float3 finalBaseNormal = float3(rgMask1, bMask1);
	
	float2 rgMask2 = AdditionalNormal.rg;
	float bMask2 = AdditionalNormal.b;
	
	rgMask2 *= -1;
	float3 finalAdditionalNormal = float3(rgMask2, bMask2);

	float dotResult1 = dot(finalBaseNormal, finalAdditionalNormal);
	float3 normalResult1 = finalBaseNormal * dotResult1;
	
	float3 normalResult2 = bMask1 * finalAdditionalNormal;
	
	return normalResult1 - normalResult2;
}

float AddComponents(float3 vInput)
{
	return float(vInput.r + vInput.g + vInput.b);
}

float3 CheapContrast_RGB(float Contrast, float3 vInput)
{
	float A = 0 - Contrast;
	float3 A3 = float3(A, A, A);
	float B = Contrast + 1;
	
	float3 result = lerp(A3, B, vInput);
	return clamp(result, 0, 1);
}

//float2 TriPlanarUV(float3 objPos, float3 normal, float3x3 TBNMatrix)
//{
//	float3 resultA = abs(objPos) / 1;
//	float2 AxRG = resultA.rg;
//	float2 AxGB = resultA.bg;
//	float2 AxRB = resultA.rb;
	
//	//float3 B = pow(abs(input.normal), 160);
//	float3 B = abs(normal);
//	float3 C = B / AddComponents(B);
//	float3 afterContrast = CheapContrast_RGB(0, C);
	
//	float2 result1 = AxRG * afterContrast.b;
//	float2 result2 = AxGB * afterContrast.r;
//	float2 result3 = AxRB * afterContrast.g;

//	float2 add1 = result2 + result3;
//	float2 finalTriPlanarUV = result1 + add1;
	
//	return finalTriPlanarUV;
//}

//float3 TriPlanarUVColor(float3 pixelWorldPos, float3 worldNormal, Texture2D texture, SamplerState textureSampler)
//{
//	float2 uvXY = pixelWorldPos.xy;
//	float2 uvZY = pixelWorldPos.zy;
//	float2 uvXZ = pixelWorldPos.xz;
	
//	float3 sampleXY = texture.Sample(textureSampler, uvXY).rgb;
//	float3 sampleZY = texture.Sample(textureSampler, uvZY).rgb;
//	float3 sampleXZ = texture.Sample(textureSampler, uvXZ).rgb;

//	return (sampleXY + sampleZY + sampleXZ) * 0.333333333333;
//}


//float3 TriPlanarUVGrayscale(float3 pixelWorldPos, float3 worldNormal, Texture2D texture, SamplerState textureSampler, int3 sampleChannel)
//{
//	float2 uvXY = pixelWorldPos.xy;
//	float2 uvZY = pixelWorldPos.zy;
//	float2 uvXZ = pixelWorldPos.xz;
	
//	float3 sampleXY = texture.Sample(textureSampler, uvXY).rgb * sampleChannel;
//	float3 sampleZY = texture.Sample(textureSampler, uvZY).rgb * sampleChannel;
//	float3 sampleXZ = texture.Sample(textureSampler, uvXZ).rgb * sampleChannel;

//	return (sampleXY + sampleZY + sampleXZ) * 0.333333333333;
//}