#version 330 core

struct LightProps
{
	int type;
	vec4 lightColor;
	vec3 lightPosition;
	vec3 dL;
	vec3 eixoCone;
	int falloff;
	float angGama;
	int eL;
};

uniform LightProps u_lights[10];

uniform mat4 u_transform;

uniform vec4 u_ambientLight;
uniform int u_flatMode;

uniform vec4 u_Oa;
uniform vec4 u_Od;
uniform vec4 u_Os;
uniform float u_shine;
uniform vec3 u_cameraPosition;
uniform int u_lightCount;

in vec4 P;

in vec3 N;
out vec4 fragmentColor;

void main()
{
  vec3 V = normalize(u_cameraPosition-vec3(P));
  vec4 A = u_ambientLight * float(1 - u_flatMode);

  vec4 oa_iA;
  vec4 refDif = vec4(0);
  vec4 refSpec = vec4(0);
  for (int i = 0; u_lightCount > i; i++)
  {
  	int type = u_lights[i].type;

  	vec3 L = u_lights[i].dL;
  	vec4 iL = u_lights[i].lightColor;
  	if(type != 0)
  	{
  		L = vec3(P) - u_lights[i].lightPosition;
  		float d = length(L);
  		L = normalize(L);
  		
			iL = u_lights[i].lightColor / pow(d, u_lights[i].falloff);
			if(type == 2)
			{
				vec3 DL = normalize(u_lights[i].eixoCone);
				float cosPsi = max(dot(DL, L), 0);
				float psi = acos(cosPsi);
				if(psi <= radians(u_lights[i].angGama))
				{
					iL = iL * pow(cosPsi, u_lights[i].eL);
				}
				else
				{
					iL = vec4(0);
				}
			}	
  	}

  	vec3 RL = reflect(-L,N);
  	RL = normalize(RL);

  	refDif = refDif + (u_Od * iL * max(dot(-L, N), float(u_flatMode)));
  	refSpec = refSpec + (u_Os * iL * pow(max(dot(-RL, V), 0), u_shine));
  }

  fragmentColor = u_Oa * u_ambientLight + refDif + refSpec;;
}