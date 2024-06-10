#version 420 core
out vec4 FragColor;

struct Material {
    sampler2D diffuseSampler;
    sampler2D specularSampler;
    sampler2D normalSampler;
    float shininess;
}; 

struct Light {
    vec3 dir;
    vec3 color; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D shadowMap;
uniform mat4 view;
float rand(vec4 seed4){
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

//in vec2 TexCoord;
in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 TangentFragPos;
in vec3 TangentViewPos;
in mat3 TBN;
in vec4 FragPosLightSpace;
uniform float useNormalMap;
uniform float useSpecularMap;
uniform float useShadow;
uniform float useLighting;
uniform float usepcf;
uniform float usecsm;
uniform int cascadeCount;
layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    float bias = max(0.05 * (1.0 - dot(Normal, light.dir)), 0.005)/100;  
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    if(usecsm > 0.5f){
        vec4 fragPosViewSpace = view * vec4(FragPos, 1.0);
        float depthValue = abs(fragPosViewSpace.z);

        int layer = -1;
        for (int i = 0; i < cascadeCount; ++i)
        {
            if (depthValue < cascadePlaneDistances[i])
            {
                layer = i;
                break;
            }
        }
        if (layer == -1)
        {
            layer = cascadeCount;
        }

        vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(FragPos, 1.0);
        // perform perspective divide
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        // transform to [0,1] range
        projCoords = projCoords * 0.5 + 0.5;

        // get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;

        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
        if (currentDepth > 1.0)
        {
            return 0.0;
        }
        // calculate bias (based on depth map resolution and slope)
        bias = max(0.05 * (1.0 - dot(normalize(Normal), light.dir)), 0.005)/10;
        if (layer == cascadeCount)
        {
            bias *= 1 / (50.0 * 0.5f);
        }
        else
        {
            bias *= 1 / (cascadePlaneDistances[layer] * 0.5f)/10;
        }

        if (layer == 0)
        {
            bias *= 1 / (50.0 * 0.5f)/1000;
        }
    }
    if(usepcf > 0.5f){
        for(int x = -1; x <= 3; ++x)
        {
            for(int y = -1; y <= 3; ++y)
            {
                int index = int(4.0*rand(vec4(gl_FragCoord.xyy, x+y+1+1)))%4;
                float pcfDepth = texture(shadowMap, projCoords.xy + poissonDisk[index] * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 25.0;
        if(projCoords.z > 1.0)
            shadow = 0.0;   
    }
    return shadow;
}

void main()
{
	vec3 color = texture(material.diffuseSampler, TexCoords).rgb;
    


    vec3 ambient =  light.color * 0.3 *texture(material.diffuseSampler, TexCoords).rgb;
    //ambient = ambient * 0.3 ;
    vec3 norm = normalize(Normal);


    if(useNormalMap > 0.5f)
	{
	    vec3 normal = texture(material.normalSampler, TexCoords).rgb;
        normal = normal * 2.0 - 1.0;   
        //normal = normalize(normal * normal); 
        norm = normal;
        norm = normalize(TBN *(norm));
	}

    vec3 lightDir = -1 * light.dir;//normalize(light.lightPos - viewPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(material.diffuseSampler, TexCoords).rgb; 
    if(diffuse[0]<0.0){
        diffuse[0]=0.0;
    }if(diffuse[1]<0.0){
        diffuse[1]=0.0;
    }if(diffuse[2]<0.0){
        diffuse[2]=0.0;
    }

    
   


    // on-off by key 3 (useLighting). 
    // if useLighting is 0, return diffuse value without considering any lighting.(DO NOT CHANGE)
	if (useLighting < 0.5f){
        FragColor = vec4(color, 1.0); 
        return; 
    }

    // on-off by key 2 (useShadow).
    // calculate shadow
    // if useShadow is 0, do not consider shadow.
    // if useShadow is 1, consider shadow.

    vec3 specular = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	
	
    
	if(useSpecularMap > 0.5f)
	{
        specular = light.color * spec * texture(material.specularSampler, TexCoords)[0];
	}

    if(specular[0]<0.0){
        specular[0]=0.0;
    }if(specular[1]<0.0){
        specular[1]=0.0;
    }if(specular[2]<0.0){
        specular[2]=0.0;
    }
    
    //else{
       // specular = specular * 0.0;
    //}
    float shadow = 0.0f;
    if(useShadow>0.5f){
        shadow = ShadowCalculation(FragPosLightSpace);
    }
    //std::cout<<shadow<<std::endl;
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));// * lightcolor;
    FragColor = vec4(lighting , 1.0);
	return;
}