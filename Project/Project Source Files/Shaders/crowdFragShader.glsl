#version 330

in vec3 vertex_in_world;
in vec3 tnorm;

// Material properties
uniform vec3 specular_intensity = vec3 (1.0, 1.0, 1.0); // specular material
uniform vec3 diffuse_intensity = vec3(1.0, 1.0, 1.0); // diffuse material
uniform vec3 ambient_intensity = vec3 (1.0, 1.0, 1.0); // ambient material

// Main light properties (directional)
uniform vec3 direction = vec3 (0.2, -1.0, 0.2); // Light position in world coords.
uniform vec3 ambient_color = vec3(0.8, 0.8, 0.8); // ambient light
uniform vec3 diffuse_color = vec3(0.9, 0.9, 0.9); // ambient light
uniform vec3 specular_color = vec3(1.0, 1.0, 1.0); // ambient light

struct PointLight {    
    vec3 position;
    // Attenuation constants
    float att_constant;
    float att_linear;
    float att_quadratic;  
    // Colors
    vec3 ambient_col;
    vec3 diffuse_col;
    vec3 specular_col;
};  
#define NR_CANDLES 2  
PointLight candleLights[NR_CANDLES];

vec3 diffuse;
vec3 ambient;
vec3 specular;

uniform vec3 camera_pos;
uniform float spec_coef = 50.0;

void createPointLights(){
    candleLights[0].position = vec3(-15.30, 8.0, 10.60);
    // Yellow-ish color
    candleLights[0].ambient_col = vec3(1.0, 1.0, 0.0);
    candleLights[0].diffuse_col = vec3(1.0, 1.0, 0.0);
    candleLights[0].specular_col = vec3(1.0, 1.0, 0.0);
    // Attenuation for 100 block distance
    candleLights[0].att_constant = 1.0;
    candleLights[0].att_linear = 0.07;
    candleLights[0].att_quadratic = 0.017; 
    
    // Similar with second
    candleLights[1].position = vec3(17.35, 5.3, -15.95);
    // Yellow-ish color
    candleLights[1].ambient_col = vec3(1.0, 1.0, 0.0);
    candleLights[1].diffuse_col = vec3(1.0, 1.0, 0.0);
    candleLights[1].specular_col = vec3(1.0, 1.0, 0.0);
    // Attenuation for 100 block distance
    candleLights[1].att_constant = 1.0;
    candleLights[1].att_linear = 0.07;
    candleLights[1].att_quadratic = 0.017; 
}

vec3 calculatePointLights(PointLight light, vec3 normal, vec3 world_vertex, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - world_vertex);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), spec_coef);
    // attenuation
    float dist = length(light.position - world_vertex);
    float attenuation = 1.0 / (light.att_constant + light.att_linear * dist + light.att_quadratic * (dist * dist));    
    // combine results
    vec3 ambient  = light.ambient_col  * ambient_intensity;
    vec3 diffuse  = light.diffuse_col  * diff * diffuse_intensity;
    vec3 specular = light.specular_col * spec * specular_intensity;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main(){
    createPointLights();
    vec3 lightDir = normalize(-direction);
    // ambient shading
    ambient = ambient_color * ambient_intensity;
  
    // The diffuse shading equation, dot product gives us the cosine of angle between the vectors
    diffuse = diffuse_color * diffuse_intensity * max( dot(tnorm, lightDir ), 0.0 );

    // specular shading
    vec3 view_dir = normalize(camera_pos - vertex_in_world);
    vec3 reflect_dir = normalize(reflect(-lightDir, tnorm));
    float spec_dot = max( dot( view_dir, reflect_dir ), 0.0 );
    float spec = pow(spec_dot, spec_coef);
    specular = specular_color * spec * specular_intensity;

    // combine shading for directional
    vec3 LightIntensity = (ambient + diffuse + specular);
    // Add point lights
    for(int i = 0; i < NR_CANDLES; i++)
        LightIntensity += calculatePointLights(candleLights[i], tnorm, vertex_in_world, view_dir);  
	gl_FragColor = vec4 (LightIntensity, 1.0);
}