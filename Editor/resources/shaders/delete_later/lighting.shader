struct Light {
	int type; // 0 = point, 1 = directional, 2 = spot
    vec3 position;
    vec3 color;
    float intensity;
	float range;
	vec3 attenuation;
	vec3 direction;
	float innerAngle;
	float outerAngle;
};



vec3 calculatePointLight(Light light, vec3 fragPos, vec3 normal) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + 
                               light.attenuation.y * distance + 
                               light.attenuation.z * distance * distance);
    float rangeFactor = max(0.0, 1.0 - distance / light.range);
    vec3 lightColor = light.color * light.intensity * rangeFactor;
    vec3 diffuse = max(dot(lightDir, normalize(normal)), 0.0) * lightColor;
    return diffuse * attenuation;
}

vec3 calculateDirectionalLight(Light light, vec3 normal) {
    vec3 lightDir = normalize(light.direction);
    vec3 lightColor = light.color * light.intensity;
    vec3 diffuse = max(dot(lightDir, normalize(normal)), 0.0) * lightColor;
    return diffuse;
}

vec3 calculateSpotLight(Light light, vec3 fragPos, vec3 normal) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + 
                               light.attenuation.y * distance + 
                               light.attenuation.z * distance * distance);

    // Calculate the angle between the light direction and the vector from the light to the fragment
    float theta = dot(lightDir, normalize(-light.direction));

    // Convert inner and outer angles to cosines for easier comparison
    float innerCos = cos(radians(light.innerAngle));
    float outerCos = cos(radians(light.outerAngle));

    // Calculate the spotlight's intensity based on the angle
    float intensity = clamp((theta - outerCos) / (innerCos - outerCos), 0.0, 1.0);

    // Apply spotlight if within the outer cone
    if (theta > outerCos) {
        vec3 lightColor = light.color * light.intensity * intensity;
        vec3 diffuse = max(dot(lightDir, normalize(normal)), 0.0) * lightColor;
        return diffuse * attenuation;
    }

    return vec3(0.0);  // Outside of the spot light's cone
}
