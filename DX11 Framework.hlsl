//--------------------------------------------------------------------------------------
// File: DX11 Framework.hlsl
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D texDiffuse : register(t0);
Texture2D texSpec: register(t1);
SamplerState sampLinear : register(s0);
TextureCube cubeMap : register(t2);

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    
    //Ambient Lightning
    float4 AmbLight;
    float4 AmbMaterial;

    //Diffuse Lightning
    float4 DiffuseLight;
    float4 DiffuseMaterial;
    float3 DirectionToLight;
    float padding;
    
    
    //Spec
    float4 SpecularMaterial;
    float4 SpecularLight;
    float SpecularPower;
    float3 EyeWorldPos;  

    float4 FogColor;        //16 Bytes
    float FogStart;         //4  Bytes
    float FogEnd;           //4  Bytes
    float pad2;
    float pad3;
    
    //int SpecularEnabled;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 PosW : POSITION0;
    float3 Normal: NORMAL0;
    float2 TexCoord : TEXCOORD0;    
    float FogFactor : FOG;
    float4 cubeMapPos : POSITION1;
    float3 cubeMapTex : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float3 Pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD )
{
    float4 pos4 = float4(Pos, 1.0f);
   
    
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( pos4, World);
    normal= normalize(normal);
    output.PosW = Pos;
    
    output.Pos = mul(output.Pos, View);
    output.Pos = mul( output.Pos, Projection );
    
    output.Normal= mul(float4(normal, 0), World);
    output.TexCoord = texCoord;
    
    //set position of skybox

    
    ////////////
    ///FOG
    ///////////
    float3 worldPos = output.PosW;
    //Calculate distance
    float fogDistance = distance(EyeWorldPos, worldPos);
    //Normalize Fog Distance
    normalize(fogDistance);
    //Linear fog
    float linearFogFactor = (FogEnd - fogDistance) / (FogEnd - FogStart);
    
    //Return Fog Factor
    output.FogFactor = linearFogFactor;
    
    output.cubeMapPos = output.Pos.xyww;
    output.cubeMapTex = output.cubeMapPos;

    
    //Calculate fog factor
    //..
    
    // float3 worldPos = output.PosW;
    // float dist = distance(EyeWorldPos, worldPos);
    //..
    // float linearFogFactor = (fogEnd - dist) / (fogEnd - fogStart);
    // float expFogFactor = 1.0 / exp(dist * fogDensity);
    // float exp2FogFactor = 1.0 / exp(pow(dist * fogDensity, 2));
    //
    // output.FogFactor = linearFogFactor;
    
    return output;
}

VS_OUTPUT SKYBOXVS(float3 Pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD)
{
    float4 pos4 = float4(Pos, 1.0f);


    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(pos4, World);
    normal = normalize(normal);
    output.PosW = Pos;

    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    output.Normal = mul(float4(normal, 0), World);
    output.TexCoord = texCoord;

    //set position of skybox


    ////////////
    ///FOG
    ///////////
    float3 worldPos = output.PosW;
    //Calculate distance
    float fogDistance = distance(EyeWorldPos, worldPos);
    //Normalize Fog Distance
    normalize(fogDistance);
    //Linear fog
    float linearFogFactor = (FogEnd - fogDistance) / (FogEnd - FogStart);

    //Return Fog Factor
    output.FogFactor = linearFogFactor;

    output.cubeMapPos = output.Pos.xyww;
    output.cubeMapTex = output.cubeMapPos;

    //Calculate fog factor
    //..

    // float3 worldPos = output.PosW;
    // float dist = distance(EyeWorldPos, worldPos);
    //..
    // float linearFogFactor = (fogEnd - dist) / (fogEnd - fogStart);
    // float expFogFactor = 1.0 / exp(dist * fogDensity);
    // float exp2FogFactor = 1.0 / exp(pow(dist * fogDensity, 2));
    //
    // output.FogFactor = linearFogFactor;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_Target
{
  
    //normalise world normall due to Floating point errors
    input.Normal = normalize(input.Normal);
    
    float diffuseAmount;
    diffuseAmount = max(dot(normalize(DirectionToLight), input.Normal), 0);
    float4 diffuse = diffuseAmount * (DiffuseMaterial * DiffuseLight);
    
    float4 ambient = AmbLight * AmbMaterial;

    
    //Normalize direction to light
    float3 lightDir = normalize(DirectionToLight);

    //Camera pos & viewer direction   
    //float3 cameraPos = float3(0.0f, 0.5f, -3.0f);
    float3 ViewerDir = EyeWorldPos - input.PosW;
    
    //
    float3 ReflectDir = normalize(reflect(-lightDir, normalize(input.Normal)));
    
    //Specular intensity 
    float SpecularIntensity = max(dot(ReflectDir, normalize(ViewerDir)), 0);
    
    //Raise it to a power 
    float SpecularFinal = pow(SpecularIntensity, SpecularPower);

    float4 specular;
    float4 textureColour;
    
    float4 totalColor = ambient + diffuse;

    bool SpecMapEnabled;
    //SpecularEnabled = true;
    SpecMapEnabled = true;
    if (SpecMapEnabled == true)
    {
        textureColour = texDiffuse.Sample(sampLinear, input.TexCoord).rgba;

        float specSample = texSpec.Sample(sampLinear, input.TexCoord).r;
        
        specular = (SpecularFinal * specSample.r) * (SpecularLight * SpecularMaterial); //specmap
        totalColor *= textureColour;
        totalColor += specular;
    }
    else if (SpecMapEnabled == false)
    {
        textureColour = texDiffuse.Sample(sampLinear, input.TexCoord);
        specular = (SpecularLight * SpecularMaterial) * SpecularFinal;
        
        totalColor *= textureColour;
        totalColor += specular;
    }
    
    
    //sample cub map texture
   // float4 cubeTexture = cubeMap.Sample(sampLinear, input.cubeMapTex);
   
    //
    // return (diffuse + ambient + specular) * textureColour + (input.fogFactor * FogColor);
    //
    
   return totalColor + (input.FogFactor * FogColor);
    


    
}
float4 SKYBOXPS(VS_OUTPUT input) : SV_Target
{

    //sample cub map texture
    float4 cubeTexture = cubeMap.Sample(sampLinear, input.cubeMapTex);
    
    return cubeTexture;
    
}
