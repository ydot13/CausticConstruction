#version 330 core
const float causticsFactor = 0.2;

in vec3 oldPosition;
in vec3 newPosition;
in float waterDepth;
in float depth;


out vec4 color;

void main() {
	float causticsIntensity = 0.;

    // intensity of caustic = S of triangle on water surf / S of triangle on underwater surf
    if (depth >= waterDepth) {
        float oldArea = length(dFdx(oldPosition).xyz) * length(dFdy(oldPosition).xyz);
        float newArea = length(dFdx(newPosition).xyz) * length(dFdy(newPosition).xyz);


        float ratio;

        // Prevent dividing by zero (debug NVidia drivers)
        if (newArea == 0.) {
            // Arbitrary large value
            ratio = 10;
        }
        else {
            ratio = oldArea / newArea;
        }
        
        causticsIntensity = causticsFactor * ratio;
    }
    color = vec4(causticsIntensity, causticsIntensity, causticsIntensity, depth);
}