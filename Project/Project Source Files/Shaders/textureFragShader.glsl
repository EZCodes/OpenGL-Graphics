#version 330

in vec2 TexCoords;
in vec3 vertex_in_world;
in vec3 tnorm;
in mat3 TBN;

// Material properties
uniform vec3 specular_intensity = vec3 (0.3, 0.3, 0.3); // specular material, not too shiny
uniform sampler2D diffuse_intensity;
uniform sampler2D normal_sampler;
uniform int hasNormals = 0;

// Main light properties (directional)
uniform vec3 direction = vec3 (0.2, -1.0, 0.2); // Light position in world coords.
uniform vec3 ambient_color = vec3(0.03, 0.03, 0.03); // ambient light
uniform vec3 diffuse_color = vec3(0.7, 0.7, 0.7); // diffuse light
uniform vec3 specular_color = vec3(0.4, 0.4, 0.4); // specular light

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
uniform float spec_coef = 20.0;

void createPointLights(){
    candleLights[0].position = vec3(-15.30, 8.0, 10.60);
    // Yellow color
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
    vec3 ambient  = light.ambient_col  * vec3(texture(diffuse_intensity, TexCoords));
    vec3 diffuse  = light.diffuse_col  * diff * vec3(texture(diffuse_intensity, TexCoords));
    vec3 specular = light.specular_col * spec * specular_intensity;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// for some reason when using normal mapping it loses diffusion, therefore I will combine it with regular lighting
vec3 calculateNormalMapping(vec3 world_vertex, vec3 viewDir) {
    vec3 normal;
    // since our model is rotated in blender, but not actually in world space
    // we dont need bitangets, but code for it is there, if need be
    //normal = texture(normal_sampler, TexCoords).rgb;
    //normal = normal * 2.0 - 1.0;   
    //normal = normalize(TBN * normal); 
        
    // obtain normal from normal map in range [0,1]
    normal = texture(normal_sampler, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(-direction);
    // ambient shading
    ambient = ambient_color * vec3(texture(diffuse_intensity, TexCoords));
  
    // The diffuse shading equation, dot product gives us the cosine of angle between the vectors
    diffuse = diffuse_color * vec3(texture(diffuse_intensity, TexCoords)) * max( dot(normal, lightDir ), 0.0 );

    // specular shading
    vec3 view_dir = normalize(camera_pos - vertex_in_world);
    vec3 reflect_dir = normalize(reflect(-lightDir, normal));
    float spec_dot = max( dot( view_dir, reflect_dir ), 0.0 );
    float spec = pow(spec_dot, spec_coef);
    specular = specular_color * spec * specular_intensity;

    // combine shading 
    return (ambient + diffuse + specular);
}

void main(){ 
    createPointLights();
    vec3 lightDir = normalize(-direction);
    // ambient shading
    ambient = ambient_color * vec3(texture(diffuse_intensity, TexCoords));
  
    // The diffuse shading equation, dot product gives us the cosine of angle between the vectors
    diffuse = diffuse_color * vec3(texture(diffuse_intensity, TexCoords)) * max( dot(tnorm, lightDir ), 0.0 );

    // specular shading
    vec3 view_dir = normalize(camera_pos - vertex_in_world);
    vec3 reflect_dir = normalize(reflect(-lightDir, tnorm));
    float spec_dot = max( dot( view_dir, reflect_dir ), 0.0 );
    float spec = pow(spec_dot, spec_coef);
    specular = specular_color * spec * specular_intensity;

    // combine shading for directional
    vec3 LightIntensity = (ambient + diffuse + specular);
    // Normal Mapping
    if (hasNormals == 1) {
        LightIntensity += calculateNormalMapping(vertex_in_world, view_dir);
    }
    // Add point lights
    for(int i = 0; i < NR_CANDLES; i++)
        LightIntensity += calculatePointLights(candleLights[i], tnorm, vertex_in_world, view_dir);  
	gl_FragColor = vec4 (LightIntensity, 1.0);
}