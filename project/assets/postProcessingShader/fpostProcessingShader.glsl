#version 400

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_ScreenTexture;

vec3 gammaCorrection(vec3 color, float gamma) {
    vec3 gc;
    gc.r = pow(color.r, gamma);
    gc.g = pow(color.g, gamma);
    gc.b = pow(color.b, gamma);
    return gc;
}

void main()
{
    float gamma = 1/1.4;
    vec3 color = texture(u_ScreenTexture, TexCoords).rgb;

    //Cromatic aberation Quellen:
    //[1] D. Lettier, „Chromatic Aberration“. Zugegriffen: 6. September 2025. [Online]. Verfügbar unter: https://lettier.github.io/3d-game-shaders-for-beginners/chromatic-aberration.html
    //[2] Tara, Game Development Stack Exchange. Zugegriffen: 6. September 2025. [Online]. Verfügbar unter: https://gamedev.stackexchange.com/a/58412

    float aberationStrength = 0.02;

    vec2 toCenter = vec2(0.5, 0.5) - TexCoords;
    float distance = length(toCenter);

    vec2 offsetR = distance * normalize(toCenter) * aberationStrength * 0.5;
    vec2 offsetG = distance * normalize(toCenter) * aberationStrength;
    vec2 offsetB = distance * normalize(toCenter) * aberationStrength * -0.5;

    float red = texture(u_ScreenTexture, TexCoords + offsetR).r;
    float blue = texture(u_ScreenTexture, TexCoords + offsetB).b;
    float green = texture(u_ScreenTexture, TexCoords + offsetG).g;
    color = vec3(red, green, blue);

    color = gammaCorrection(color, gamma);


    FragColor = vec4(color, 1.0);
    //sjkfhsdjkfhsjkd
}