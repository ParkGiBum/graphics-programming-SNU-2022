﻿#include "Particle.h"
#include <random>
#include <time.h>
#include "Flame.h"


int reset;
using namespace glm;
float s = 2.f;
static vec3 curl3;
vec4 permute(vec4 x) { return mod(((x * vec4(34.0)) + vec4(1.0)) * x, vec4(289.0)); }
vec4 taylorInvSqrt(vec4 r) { return vec4(1.79284291400159) - vec4(0.85373472095314) * vec4(r); }
float snoise(vec3 v) {
	const vec2  C = vec2(1.0 / 6.0, 1.0 / 3.0);
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);
	// First corner
	vec3 i = floor(v + dot(v, vec3(C.y, C.y, C.y)));
	vec3 x0 = v - i + dot(i, vec3(C.x, C.x, C.x));

	// Other corners
	vec3 g = vec3(0.0f);
	if (x0.y < x0.x) {
		g.x = 1;
	}if (x0.z < x0.y) {
		g.x = 1;
	}if (x0.x < x0.z) {
		g.x = 1;
	}

	vec3 l = vec3(1.0) - g;
	vec3 i1 = min(vec3(g.x, g.y, g.z), vec3(l.z, l.x, l.y));
	vec3 i2 = max(vec3(g.x, g.y, g.z), vec3(l.z, l.x, l.y));
	//  x0 = x0 - 0. + 0.0 * C
	vec3 x1 = x0 - i1 + vec3(1.0) * vec3(C.x, C.x, C.x);
	vec3 x2 = x0 - i2 + vec3(2.0) * vec3(C.x, C.x, C.x);
	vec3 x3 = x0 - vec3(1.0) + vec3(3.0) * vec3(C.x, C.x, C.x);
	// Permutations
	i = mod(i, vec3(289.0));
	vec4 p = permute(permute(permute(
		i.z + vec4(0.0, i1.z, i2.z, 1.0))
		+ i.y + vec4(0.0, i1.y, i2.y, 1.0))
		+ i.x + vec4(0.0, i1.x, i2.x, 1.0));
	// Gradients
	// ( N*N points uniformly over a square, mapped onto an octahedron.)
	float n_ = 1.0 / 7.0; // N=7
	vec3  ns = n_ * vec3(D.w, D.y, D.z) - vec3(D.x, D.z, D.x);
	vec4 j = p - vec4(49.0) * floor(p * ns.z * ns.z);  //  mod(p,N*N)
	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - vec4(7.0) * x_);    // mod(j,N)
	vec4 x = x_ * ns.x + vec4(ns.y, ns.y, ns.y, ns.y);
	vec4 y = y_ * ns.x + vec4(ns.y, ns.y, ns.y, ns.y);
	vec4 h = vec4(1.0) - abs(x) - abs(y);
	vec4 b0 = vec4(x.x, x.y, y.x, y.y);
	vec4 b1 = vec4(x.z, x.w, y.z, y.w);
	vec4 s0 = floor(b0) * vec4(2.0) + vec4(1.0);
	vec4 s1 = floor(b1) * vec4(2.0) + vec4(1.0);
	vec4 sh = -step(h, vec4(0.0));
	vec4 a0 = vec4(b0.x, b0.z, b0.y, b0.w) + vec4(s0.x, s0.z, s0.y, s0.w) * vec4(sh.x, sh.x, sh.y, sh.y);
	vec4 a1 = vec4(b1.x, b1.z, b1.y, b1.w) + vec4(s0.x, s0.z, s0.y, s0.w) * vec4(sh.z, sh.z, sh.w, sh.w);
	vec3 p0 = vec3(a0.x, a0.y, h.x);
	vec3 p1 = vec3(a0.z, a0.w, h.y);
	vec3 p2 = vec3(a1.x, a1.y, h.z);
	vec3 p3 = vec3(a1.z, a1.w, h.w);
	//Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	vec4 m = max(vec4(0.6) - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), vec4(0.0));
	m = m * m;
	return 42.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}

const float PI = 3.1415926535897932384626433832795;
float rnd(vec2 co)
{
	return fract(sin(mod((dot(vec2(co.x, co.y), vec2(12.9898, 78.233))), (3.14f))) * (43758.5453));
}

vec3 snoise3(vec3 p)
{
	return vec3(snoise(p),
		snoise(vec3(p.y + 31.416f, p.z - 47.853f, p.x + 12.793f)),
		snoise(vec3(p.z - 233.145f, p.x - 113.408f, p.y - 185.31f)));
}


const float EPS = 1e-4f;

vec3 curl(float s,vec3 m_position)
{
	vec3 dx = vec3(EPS, 0.f, 0.f);
	vec3 dy = vec3(0.f, EPS, 0.f);
	vec3 dz = vec3(0.f, 0.f, EPS);

	vec3 dx0 = snoise3((m_position - dx) * s);
	vec3 dx1 = snoise3((m_position + dx) * s);
	vec3 dy0 = snoise3((m_position - dy) * s);
	vec3 dy1 = snoise3((m_position + dy) * s);
	vec3 dz0 = snoise3((m_position - dz) * s);
	vec3 dz1 = snoise3((m_position + dz) * s);

	float x = dy1.z - dy0.z - dz1.y + dz0.y;
	float y = dz1.x - dz0.x - dx1.z + dx0.z;
	float z = dx1.y - dx0.y - dy1.x + dy0.x;
	return normalize(vec3(x, y, z) * 1.f / (2.f * EPS));
}


//contructor giving a random starting position (within range)
Particle::Particle(int type2) 
{
	type = type2;
	total_time = 0;
	m_lifetime = 50000/2;
	m_velocity = glm::vec3(0,0,0);
	m_position = glm::vec3((((float) rand() / (float) RAND_MAX) - 0.5),0.1,(((float) rand() / (float) RAND_MAX) - 0.5));
	if (type == 2) {
		m_velocity = glm::vec3((((float)rand() / (float)RAND_MAX) - 0.5), (((float)rand() / (float)RAND_MAX) - 0.5) * 5.0f, (((float)rand() / (float)RAND_MAX) - 0.5));
		m_velocity *= glm::vec3(2.0f);

	}

}

void collision(vec3 * position,vec3 * veloctiy) {
	float dist = glm::distance(vec2(1.0, .2), vec2(position->y, position->z));
	float tmp = 1.0;
	if (dist < 0.5f) {
		vec3 way = glm::cross(vec3(1.0,0,0),vec3(0.0, position->y - 1, position->z - 0.2));
		if (way.y < 0) {
			way = vec3(-1) * way;
		}
		if (position->y > 1.0f) {
			tmp = 0.1f;
		}
		float(size) = sqrt((veloctiy->y)* (veloctiy->y) + (veloctiy->z) * (veloctiy->z) + (veloctiy->x) * (veloctiy->x) );
		veloctiy->x = way.x * size*tmp;
		veloctiy->y = way.y * size;
		veloctiy->z = way.z * size*tmp;
	}


}
//update loop goes through all of the particles within a small range of itself and either accelerates towards or away from them depending on density
//though not quite a full fluid dynamics equation it gives a good substitute at a low cost to the cpu
void Particle::Update(std::vector<Particle*> &_particlesInRange, unsigned int _deltaTime)
{
	if (m_position.y > 1.5f) {
		m_lifetime -= _deltaTime; m_lifetime -= _deltaTime; m_lifetime -= _deltaTime;
	}
	total_time += _deltaTime;
	glm::vec3 acceleration;
	//upwards acceleration due to convection added
	acceleration.y += 0.6f;
	//lifetime of the particle decreased
	m_lifetime -= _deltaTime;
	//for each particle in range move towards or away from it depending on density 
	for (unsigned int i = 0; i < _particlesInRange.size(); i++)
	{
		if (_particlesInRange[i] != this)
		{	

			if (_particlesInRange.size() > 20)
			{
				acceleration += (m_position - _particlesInRange[i]->m_position) * 0.02f;
			}
			else
			{
				acceleration += (m_position - _particlesInRange[i]->m_position) * -2.0f;
			}
		}
	}
	

	if (m_position.y > 0.5f) {
		if (m_velocity.x > 0)
			m_velocity.x -= m_position.x / 2;
		else
			m_velocity.x += m_position.x / 2;
		if (m_velocity.x > 0)
			m_velocity.x -= m_position.z / 2;
		else
			m_velocity.x += m_position.z;
	}
	reset = reset + 1;
	vec3 curl2;
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if (type == 1 & total_time < 5000000000) {
		if (reset > 0 || !curl3.x) {
			curl2 = curl(2.0f, m_position ) * vec3(0.5f);// +curl(4.0f, m_position) * vec3(0.25f);
			reset = 0;
		}
		else {
			curl2 = curl3;
		}
		curl3 = curl2;
		vec3 velocity = vec3(0.045 * (rnd(vec2(total_time, total_time + 1)) * 2.f - 1), .35f, 0.045 * (rnd(vec2(total_time * 1000, total_time * 91)) * 2.f - 1));

		m_velocity = velocity + curl2;
	}
	else {

		m_velocity.y = m_velocity.y - 0.1f;
		if (m_position.y < 0.1f) {

			m_velocity.y = m_velocity.y * -1;
		}

	}
	
	collision(&m_position, &m_velocity);
	//move the particle
	if(total_time < 5000000000)
		m_position += m_velocity * ((float)_deltaTime / 10000.0f); //* vec3(type * type * type * type * type * 5);
	else {
		m_lifetime = 100.0f;
	}
}


