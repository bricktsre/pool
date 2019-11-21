#version 120

varying vec4 N;
varying vec4 L;
varying vec4 V;
varying vec4 newPosition;
varying float distance;

uniform vec4 ambient_product, diffuse_product, specular_product;
uniform float shininess;
uniform float attenuation_constant, attenuation_linear, attenuation_quadratic;
uniform int useTexture;
uniform sampler2D myTextureSampler;
vec4 ambient, diffuse, specular;

mat4 bias = mat4(vec4(0.5,0.0,0.0,0.0),vec4(0.0,0.5,0.0,0.0),vec4(0.0,0.0,-0.5,0.0),vec4(0.5,0.5,0.5,1.0));

void main()
{
	if(useTexture == 1){
		gl_FragDepth = gl_FragCoord.z;
	}else{
		vec4 newFragPosition = bias * (newPosition / newPosition.w);
		vec4 NN = normalize(N);
		vec4 VV = normalize(V);
		vec4 LL = normalize(L);
		ambient = ambient_product;
		vec4 H = normalize(LL + VV);
		diffuse = max(dot(LL, NN), 0.0) * diffuse_product;
		specular = pow(max(dot(NN, H), 0.0), shininess) * specular_product;
		float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) + (attenuation_quadratic * distance * distance));
		if(newFragPosition.z <= texture2D(myTextureSampler,vec2(newFragPosition.x, newFragPosition.y)).z+0.01)
			gl_FragColor = ambient + (attenuation * (diffuse + specular));
		else
			gl_FragColor = 0.2 * (ambient + (attenuation * (diffuse + specular)));
	}
}
