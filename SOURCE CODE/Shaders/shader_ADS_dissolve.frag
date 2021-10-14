#version 440 core
out vec4 vertColour;
in vec2 textureCoordinate;
in vec3 normals;
in vec3 fragmentPosition;
in vec3 lightColour;
in vec3 lightPosition;
in vec3 viewPosition;
in vec3 vPosition; //xyz coordinates relative to model, not world (used for one effect)
in float time;


uniform sampler2D moonTex;
uniform sampler2D lavaMoonTex;
uniform sampler2D transitionTex;

void main()
{	
	// TEXTURES--------------------------------------------------------------------------------------------------------
	
	// #  'Normal' Moon, downloaded from:
	// https://www.solarsystemscope.com/textures/ @ 2k resolution
	vec4 moonTextureColour = texture(moonTex, textureCoordinate);
	
	// #  Molten Moon, created using:
	// The previous 'normal moon' texture and a modified 'cracks' image, downloaded from:
	// https://unsplash.com/photos/_JgMnccR3Q4
	// These were edited together in Photoshop to produce the final 'Molten Moon' texture.
	vec4 lavaMoonTextureColour = texture(lavaMoonTex, textureCoordinate);
	
	// #  Perlin Noise, generated then downloaded from:
	// http://www.kitfox.com/projects/perlinNoiseMaker/index.html 
	// Site generation settings: (25, 2000, 1000, 150, 1, 4, alpha checked only)
	vec4 perlinTexture = texture(transitionTex, textureCoordinate);
	
	
	// SHARED COMPONENTS-----------------------------------------------------------------------------------------------
	// These are components used by both visual appearances (Molten & 'Normal' Moon).
	
	// #  Ambient Light Component (Largely unchanged from Professor Doughty's implementation)--------------------------#
	float lightAmbientStrength = 0.15f;
	vec3 objectAmbientReflectionCoeff = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambient = (lightAmbientStrength * objectAmbientReflectionCoeff) * lightColour;
	
	
	// #  Diffuse Light Component (Largely unchanged from Professor Doughty's implementation)--------------------------#
	vec3 nNormal = normalize(normals);
	vec3 lightDirection = normalize(lightPosition - fragmentPosition);
	float diffuseStrength = max(dot(nNormal, lightDirection), 0.0f);
	vec3 objectDiffuseReflectionCoeff = vec3(1.0f, 1.0f, 1.0f);
    vec3 diffuse = (diffuseStrength * objectDiffuseReflectionCoeff) * lightColour;
	
	
	// #  Dissolve Effect Base Component------------------------------------------------------------------------------#
	// Dissolves can be done in many ways, though they appear most 'organic' when a form of noise is used to rig them.
	// This dissolve effect has no particular origin and unfortunately does not wrap, but, it is mapped for a sphere using a Photoshop Plugin.
	// An animation resembling an asteroid impact *could* have been created if the alpha values are treated as 'frames' in the impact animation;
	// imagine a ring of red-white radiating out from a central (impact) point.
	// This is believed to be outside the scope of the assignment since it does not concern OpenGL or shaders; it has not been done.
	
	//   waveSix is 1/6Hz wave that takes sdlTicks time uniform.
	// Smoothstepping could be done here or at dissolveTime, but it would make little difference.
	// 1.0 is added within the time component so that the dissolve effect begins on the 'normal' moon instead of halfway between both textures.
	float waveSix = sin(1+(time/6000));
	
	//   dissolveTime is the offsetting of waveSix with the perlin texture's alpha Value.
	// The calculation is offset by -1.0f so that the animation rigging by the perlin noise is centred about '0.0'.
	// The tex.Alpha value could be mathematically modified here to give a different gradient (influencing the final effect).
	// It would then require smoothstepping then recentering about zero for such an implementation.
	float dissolveTime = waveSix + perlinTexture.a - 1.0f;
	
	//   dissolveAnimation Defines the 'edges' of the dissolve effet for use with the two textures.
	// When dissolveTime exceeds 0.0 we see the 'Normal' Moon, below 0.0 we see the Molten Moon.
	// Of course this is now offsetted by perlinTexture's alpha value, so different areas will transition at different times.
	float dissolveAnimation = step(0.0f, dissolveTime);
	
	// #  Dissolve Effect, 'Burnline' Component-----------------------------------------------------------------------#
	// This makes a 'red hot' effect either side of where the dissolve's edge occurs.
	// There is an additional 'white hot' appearing line to create a 'hot-vhot-hot' appearance.
	// The dissolveAnimation modifiers' gradient cannot be used here because it is stepped instead of smoothstepped
	// (we cannot specify any location other than the exact boundry, which would have no width).
	// Each step() for this component sets a floor / ceiling for the red & white hot edges involved.
	// Red hot's vec3 has overall lower RGB values (BG: 0.1), while White hot's has overall higher RGB values (BG: 0.6) for a colour closer to 'white-hot'.
	vec3 burnLine = (vec3(1.0f, 0.1f, 0.1f) * (step(-0.04f, dissolveTime) - step(+0.04f, dissolveTime)) + 
					vec3(1.0f, 0.6f, 0.6f) * (step(-0.02f, dissolveTime) - step(0.02f, dissolveTime)));
	
	
	// UNIQUE COMPONENTS-----------------------------------------------------------------------------------------------
	// Components unique to each appearance
	
	// #  Normal Moon, Specular Component (Largely unchanged from Professor Doughty's implementation)------------------#
	float specularStrength = 0.1f;
	vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, nNormal); 
	float sp = pow(max(dot(viewDirection, reflectDirection), 0.0f), 128);
    vec3 specular = specularStrength * sp * lightColour;
	
	
	// #  Molten Moon, Equatorial 'Glowing' Cracks Component----------------------------------------------------------#
	// This is essentially an animated ambient-light component limited to Molten Moon fragments with tex.Red values above 0.27.
	// An alternative could give the 'lava cracks' its own texture instead of checking for 'Redness'.
	// The checking of the tex.Red value using step() interesting, and fewer textures are used; it was kept.
	// This has no contribution to the overall Dissolve effect, but is very cool and took many hours to get right.
	
	//   fragIsLava Modifier serves as 'bool' for whether the current frag (as applied to the Molten Moon texture) has a Red intensity above 0.27.
	// The cutoff threshold of 0.27 was determined by inspecting the colour curves of the texture on photoshop.
	float fragIsLava = step(0.27f, lavaMoonTextureColour.r);
	
	//   glowAnimation Modifier varies the fragments's Alpha value over time, originating at the equator by using the model vertex's y coords.
	// Previously, movement along the y axis /was/ accounted for using an Offset uniform (with fragmentPosition.y.
	// Using vertex coordinates offers a more robust solution that accounts for any transforms.
	// We're taking the /absolute/ value of the vertexPosition.y because we want to use /distance/ from equator as a modifier,
	// Eg: The UK doesn't have a 'negative distance' from the equator, and nor does South Africa.
	// The smoothstep's 3rd paramter only actually ranges from -0.5 to +0.5, so
	// setting the smoothstep edges at -0.8 and +0.8 means the glow is never at 0% or 100% intensity, this achieves the intended appearance.
	float glowAnimation = smoothstep(-0.8f, 0.8f, (sin(time/1000)/4) - abs(vPosition.y));
	
	//   lavaGlow is the consolidation of Glowing 'Cracks' component, multiplied by a 'full' vec3 coefficient as is done for a normal diffuse component.
	vec3 lavaGlow = vec3(1.0f, 1.0f, 1.0f) * glowAnimation * fragIsLava;	
	
	
	// CONSOLIDATION AND OUTPUT----------------------------------------------------------------------------------------
	
	// #  Consolidation of components for Molten and Normal appearance for readability--------------------------------#
	// Burnline is applied to both textures so it overlaps them;
	// applying the Burnline to just one would only show the effect on one 'edge' due to the stepping involved
	// The Molten Moon is intended to be less 'shiny' owing to the ash on the surface, hence the excluded specular component.
	vec4 vertColourLava = vec4((ambient + diffuse + lavaGlow + burnLine),1.0f) * lavaMoonTextureColour;
	vec4 vertColourMoon = vec4((ambient + diffuse + specular + burnLine),1.0f) * moonTextureColour;
	
	
	// #  Fragment Shader Output--------------------------------------------------------------------------------------#
	vertColour = (vertColourLava * (1.0f - dissolveAnimation)) + (vertColourMoon * dissolveAnimation);
}





