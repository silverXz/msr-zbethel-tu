///////////////////////////////////////////////////////////////////////
//
// Multithreaded Software Rasterizer
// Copyright 2010 :: Zach Bethel 
//
///////////////////////////////////////////////////////////////////////

#include "MSR_Shader.h"
#include "MSR_Internal.h"

//
// Vertex Processing
//

void ProcessVertexGeneric(MSR_Vertex *v, MSR_TransformedVertex *v_trans) 
{
/*	bool useLighting = useNormal && (lightMode != MSR_LIGHTING_NONE);
	bool useGouraud  = useNormal && (lightMode == MSR_LIGHTING_GOURAUD);
	*/
	// Translate world vector
	v_trans->p = render_context.globals.wvp * v->p; 
	
/*	// Get color space vector
	MSR_Color *c_out = (MSR_Color*)&v_trans->data[COLOR_SPACE_R];

	// Get the specular color space vector
	MSR_Color *s_out = (MSR_Color*)&v_trans->data[SPEC_SPACE_R];
	
	// Copy color if we aren't lighting
	if( useColor && !useGouraud ) {
		c_out->r = v->c.r;
		c_out->g = v->c.g;
		c_out->b = v->c.b;
	}

	// Flat and Gouraud both use the same lighting equations
	if( useLighting ) 
	{
		//
		// Per-Vertex Lighting
		//

		c_out->r = c_out->g = c_out->b = 0.0f;
		s_out->r = s_out->g = s_out->b = 0.0f;

		// Get the eye coordinates from the current view matrix. They should just be the inverted position.
		MSR_Vec4 eye(render_context.viewinv._41,render_context.viewinv._42,render_context.viewinv._43);

		// Transform position to world
		MSR_Vec4 p_world = render_context.world * v->p;

		// Get the eye to point vector
		MSR_Vec4 v_eye = eye - p_world;
		v_eye.Normalize();

		// Transform normal to world (use the 3x3 portion only)
		MSR_Vec4 n_world = render_context.world.TransformNormal(v->n);
		n_world.Homogenize();
		n_world.Normalize();

		// Loop through lights
		MSR_Material &material = render_context.material;
		for( Uint32 curr_light=0; curr_light<MSR_MAX_LIGHTS; curr_light++ )
		{
			if( !render_context.lights_enabled[curr_light] ) continue;
			MSR_Light &light = render_context.lights[curr_light];

			if( light.type == MSR_LIGHT_TYPE_AMBIENT )
			{
				c_out->r += render_context.ml_ambient[curr_light].r;
				c_out->g += render_context.ml_ambient[curr_light].g;
				c_out->b += render_context.ml_ambient[curr_light].b;
			}
			else if( light.type == MSR_LIGHT_TYPE_POINT ) 
			{
				// Get the light to point vector
				MSR_Vec4 v_light = light.position - p_world;
				float dist = v_light.Length();
				float inv_dist = 1.0f / dist;
				if( dist > light.range ) continue;
				v_light.x *= inv_dist; v_light.y *= inv_dist; v_light.z *= inv_dist;

				// Compute diffuse term
				float nl = n_world * v_light;
				if( nl < 0.0f ) nl = 0.0f; 

				// Compute the light attenuation
				float att = 1.0f/(light.attenuation0 + 
								  light.attenuation1 * dist + 
								  light.attenuation2 * dist * dist);

				// Compute specular term
				MSR_Vec4 reflect = (2.0f * nl) * n_world - v_light;
				reflect.Normalize();
				float rv = reflect * v_eye;
				if( rv < 0.0f ) rv = 0.0f;
				rv = powf( rv, render_context.material.power );

				float df = nl*att;
				float sf = att*rv;

				// Compute final colors
				c_out->r += render_context.ml_ambient[curr_light].r + render_context.ml_diffuse[curr_light].r * df;
				c_out->g += render_context.ml_ambient[curr_light].g + render_context.ml_diffuse[curr_light].g * df;
				c_out->b += render_context.ml_ambient[curr_light].b + render_context.ml_diffuse[curr_light].b * df;
	
				s_out->r += render_context.ml_specular[curr_light].r * sf;
				s_out->g += render_context.ml_specular[curr_light].g * sf;
				s_out->b += render_context.ml_specular[curr_light].b * sf;
			} 
			else if( light.type == MSR_LIGHT_TYPE_DIRECTIONAL ) 
			{
				// Compute diffuse term
				float nl = n_world * -light.direction;
				if( nl < 0.0f ) nl = 0.0f; 
					
				// Compute specular term
				MSR_Vec4 reflect = (2.0f * nl) * n_world + light.direction;
				reflect.Normalize();
				float rv = reflect * v_eye;
				if( rv < 0.0f ) rv = 0.0f;
				rv = powf( rv, render_context.material.power );

				// Compute final colors
				c_out->r += render_context.ml_ambient[curr_light].r + render_context.ml_diffuse[curr_light].r * nl;
				c_out->g += render_context.ml_ambient[curr_light].g + render_context.ml_diffuse[curr_light].g * nl;
				c_out->b += render_context.ml_ambient[curr_light].b + render_context.ml_diffuse[curr_light].b * nl;

				s_out->r += render_context.ml_specular[curr_light].r * rv;
				s_out->g += render_context.ml_specular[curr_light].g * rv;
				s_out->b += render_context.ml_specular[curr_light].b * rv;
			} 
			else if( light.type == MSR_LIGHT_TYPE_SPOT ) 
			{
				// Get the light to point vector
				MSR_Vec4 v_light = light.position - p_world;
				float dist = v_light.Length();
				if( dist > light.range ) continue;
				v_light.x /= dist; v_light.y /= dist; v_light.z /= dist;

				// Compute diffuse term
				float nl = n_world * v_light;
				if( nl < 0.0f ) nl = 0.0f; 

				// Compute angles
				float alpha = -light.direction * v_light;
				float cos_theta_half = cosf(light.theta * 0.5f);
				float cos_phi_half = cosf(light.phi * 0.5f);
				float spot_factor;

				if( alpha <= cos_phi_half ) 
					spot_factor = 0.0f;
				else if( alpha > cos_theta_half ) 
					spot_factor = 1.0f;
				else {

					// Compute falloff
					spot_factor = (alpha-cos_phi_half)/(cos_theta_half-cos_phi_half);
					spot_factor = pow(spot_factor,light.falloff);
				}

				// Compute the light attenuation
				float att = 1.0f/(light.attenuation0 + 
								  light.attenuation1 * dist + 
								  light.attenuation2 * dist * dist);

				// Compute specular term
				MSR_Vec4 reflect = (2.0f * nl) * n_world - v_light;
				reflect.Normalize();
				float rv = reflect * v_eye;
				if( rv < 0.0f ) rv = 0.0f;
				rv = powf( rv, render_context.material.power );

				float df = (nl*att*spot_factor);
				float sf = (att*rv*spot_factor);

				// Compute final colors
				c_out->r += render_context.ml_ambient[curr_light].r + render_context.ml_diffuse[curr_light].r * df; 
				c_out->g += render_context.ml_ambient[curr_light].g + render_context.ml_diffuse[curr_light].g * df;
				c_out->b += render_context.ml_ambient[curr_light].b + render_context.ml_diffuse[curr_light].b * df;

				s_out->r += render_context.ml_specular[curr_light].r * sf;
				s_out->g += render_context.ml_specular[curr_light].g * sf;
				s_out->b += render_context.ml_specular[curr_light].b * sf;
			} 
		}

		if( useColor && useGouraud ) {
			c_out->r *= v->c.r;
			c_out->g *= v->c.g;
			c_out->b *= v->c.b;
		}

		if(c_out->r > 1.0f) c_out->r = 1.0f;
		if(c_out->g > 1.0f) c_out->g = 1.0f;
		if(c_out->b > 1.0f) c_out->b = 1.0f;

		if(s_out->r > 1.0f) s_out->r = 1.0f;
		if(s_out->g > 1.0f) s_out->g = 1.0f;
		if(s_out->b > 1.0f) s_out->b = 1.0f;
	}

	// Copy texture
	if( useTexture ) {
		v_trans->data[TEXTURE_SPACE_U]	= v->u;
		v_trans->data[TEXTURE_SPACE_V]	= v->v;
	}*/
}

//
// 
//

static inline __m128i mul_epi32(__m128i &a, __m128i &b)
{
	__m128i tmp1 = _mm_mul_epu32(a,b);
	__m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4));
	return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0)));
}

// 
// Pixel Processing
//

static inline void compute_gradient( float C, 
			  					     float di21, float di31, 
								     float dx21, float dx31,
								     float dy21, float dy31,
								     float &dx, float &dy ) {

	float A = di31 * dy21 - di21 * dy31;
	float B = dx31 * di21 - dx21 * di31;

	dx = -A/C;
	dy = -B/C;
}

/*#define SET_R(useZBuffer,useColor,useNormal,useTexture,lightMode) \
	(ProcessVertex) = (ProcessVertexGeneric<useZBuffer,useColor,useNormal,useTexture,lightMode>); \
	(RenderFragments) = (ProcessFragmentGeneric<useZBuffer,useColor,useNormal,useTexture,lightMode>);

void PrepareRasterizer()
{
	//
	// Set the vertex and fragment shaders to what they need to be
	//

	switch( render_context.fvf ) 
	{
	case MSR_FVF_COLOR:
		{
			if( render_context.depth_enabled ) 
			{
				SET_R(1,1,0,0,MSR_LIGHTING_NONE);
			}
			else 
			{
				SET_R(0,1,0,0,MSR_LIGHTING_NONE);
			}
			break;
		}
	case MSR_FVF_NORMAL:
		{
			if( render_context.light_mode == MSR_LIGHTING_GOURAUD ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,0,MSR_LIGHTING_GOURAUD);
				} 
				else 
				{
					SET_R(0,0,1,0,MSR_LIGHTING_GOURAUD);
				}
			} 
			else if( render_context.light_mode == MSR_LIGHTING_FLAT ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,0,MSR_LIGHTING_FLAT);
				} 
				else 
				{
					SET_R(0,0,1,0,MSR_LIGHTING_FLAT);
				}
			} 
			else 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,0,MSR_LIGHTING_NONE);
				} 
				else 
				{
					SET_R(0,0,1,0,MSR_LIGHTING_NONE);
				}
			}
			break;
		}
	case MSR_FVF_TEX_UV:
		{
			if( render_context.depth_enabled ) 
			{
				SET_R(1,0,0,1,MSR_LIGHTING_NONE);
			} 
			else 
			{
				SET_R(0,0,0,1,MSR_LIGHTING_NONE);
			}
			break;
		}
	case MSR_FVF_COLOR|MSR_FVF_NORMAL:
		{
			if( render_context.light_mode == MSR_LIGHTING_GOURAUD ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,0,MSR_LIGHTING_GOURAUD);
				} 
				else 
				{
					SET_R(0,0,1,0,MSR_LIGHTING_GOURAUD);
				}
			} 
			else if( render_context.light_mode == MSR_LIGHTING_FLAT ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,0,MSR_LIGHTING_FLAT);
				} 
				else 
				{
					SET_R(0,0,1,0,MSR_LIGHTING_FLAT);
				}
			} 
			else 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,0,MSR_LIGHTING_NONE);
				} 
				else 
				{
					SET_R(0,0,1,0,MSR_LIGHTING_NONE);
				}
			}
			break;
		}
	case MSR_FVF_COLOR|MSR_FVF_TEX_UV:
		{
			if( render_context.depth_enabled ) 
			{
				SET_R(1,1,0,1,MSR_LIGHTING_NONE);
			} 
			else 
			{
				SET_R(0,1,0,1,MSR_LIGHTING_NONE);
			}
			break;
		}
	case MSR_FVF_NORMAL|MSR_FVF_TEX_UV:
		{
			if( render_context.light_mode == MSR_LIGHTING_GOURAUD ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,1,MSR_LIGHTING_GOURAUD);
				} 
				else 
				{
					SET_R(0,0,1,1,MSR_LIGHTING_GOURAUD);
				}
			} 
			else if( render_context.light_mode == MSR_LIGHTING_FLAT ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,1,MSR_LIGHTING_FLAT);
				} 
				else 
				{
					SET_R(0,0,1,1,MSR_LIGHTING_FLAT);
				}
			} 
			else
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,0,1,1,MSR_LIGHTING_NONE);
				} 
				else 
				{
					SET_R(0,0,1,1,MSR_LIGHTING_NONE);
				}
			}
			break;
		}
	case MSR_FVF_COLOR|MSR_FVF_NORMAL|MSR_FVF_TEX_UV:
		{
			if( render_context.light_mode == MSR_LIGHTING_GOURAUD ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,1,1,1,MSR_LIGHTING_GOURAUD);
				} 
				else 
				{
					SET_R(0,1,1,1,MSR_LIGHTING_GOURAUD);
				}
			} 
			else if( render_context.light_mode == MSR_LIGHTING_FLAT ) 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,1,1,1,MSR_LIGHTING_FLAT);
				} 
				else 
				{
					SET_R(0,1,1,1,MSR_LIGHTING_FLAT);
				}
			} 
			else 
			{
				if( render_context.depth_enabled ) 
				{
					SET_R(1,1,1,1,MSR_LIGHTING_NONE);
				} 
				else 
				{
					SET_R(0,1,1,1,MSR_LIGHTING_NONE);
				}
			}

			break;
		}
	default:
		if( render_context.depth_enabled ) 
		{
			SET_R(1,0,0,0,MSR_LIGHTING_NONE);
		} 
		else 
		{
			SET_R(0,0,0,0,MSR_LIGHTING_NONE);
		}
	}
}*/