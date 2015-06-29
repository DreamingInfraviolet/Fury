cbuffer Uniforms
{
	float4 ObjectColour;
}

struct PixelInputType
{
	float4 positionProjectionspace : SV_POSITION;
	float4 positionCameraspace : POSITION;
	float3 normalCameraspace : NORMAL;
	float3 lightDirection : POSITION1;
	float2 uv : TEXCOORD0;
};

Texture2D diffuseTexture;
SamplerState samplerState;



static const float PI = 3.141592653589793238462643;

///////////////////////////// 

static const float m=0.2; //Rougness [0,1]
static const float m2 = m*m;
static const float F0 =0.8;

float D(float nh)
{
float nh2 = nh*nh;
return 1/(m2*nh2*nh2) * exp ((nh*nh-1.0)/( m2*nh2));
}

float F(float vh)
{
return F0+(1-F0)*pow(1-vh,5);
}

float G(float nv, float nl, float nh, float vh)
{
float nh2 = nh*2;
return min(1, min((nh2*nv)/vh, (nh2*nl)/vh));
}

float CookTorrance(float nh, float vh, float nv, float nl)
{
return (D(nh)*F(vh)*G(nv,nl,nh,vh))/(4*nl*nv);
}


float ComputeDiffuse(const in float nv, const in float nl, const in float3 l, const in float3 v, const in float3 n)
{    
      float angleVN = acos(nv);
    float angleLN = acos(nl);
    
    float alpha = max(angleVN, angleLN);
    float beta = min(angleVN, angleLN);
    float gamma = dot(v - n * nv, l - n * nl);
    
    float roughnessSquared9 = (m2 / (m2 + 0.09));
    
    // calculate C1, C2 and C3
    float C1 = 1.0 - 0.5 * (m2 / (m2 + 0.33));
    float C2 = 0.45 * roughnessSquared9;
    
    if(gamma >= 0.0)
    {
        C2 *= sin(alpha);
    }
    else
    {
        C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
    }
 
    float powValue = (4.0 * alpha * beta) / (PI * PI);
    float C3  = 0.125 * roughnessSquared9 * powValue * powValue;
 
    // now calculate both main parts of the formula
    float A = gamma * C2 * tan(beta);
    float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);
 
    // put it all together
    float L1 = max(0.0, nl) * (C1 + A + B);
	
	// also calculate interreflection
    float twoBetaPi = 2.0 * beta / PI;
	//TODO: p is squared in this case... how to separate this?
    float L2 = 0.17 * max(0.0, nl) * (m2 / (m2 + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);
    
    // get the final color by summing both components
    return(L1 + L2);
}


float4 main(PixelInputType input) : SV_TARGET
{
	float3 n = normalize(input.normalCameraspace.xyz);
	float3 v = normalize(-input.positionCameraspace.xyz);
	float3 l = normalize(input.lightDirection);
	float3 h = normalize(v+l);
	
	float nh = dot(n,h); float vh = dot(v,h);
	float nv = dot(n,v); float nl = dot(n,l);
	
	
	float specularMultiplier = 0.1;
	float specularCoefficient = 0;
	
	float diffuseCoefficient = max(0,ComputeDiffuse(nv, nl, l, v, n));
	
	if (diffuseCoefficient>0)
		specularCoefficient = CookTorrance(nh,vh,nv,nl);
	
	float3 diffuse = diffuseCoefficient;
	float3 ambient = 0.0;
	float3 specular = specularCoefficient*specularMultiplier;
	
	float3 colour = diffuseTexture.Sample(samplerState, input.uv).xyz*(diffuse*ObjectColour.xyz + specular + ambient);
	return float4(colour,1);
}