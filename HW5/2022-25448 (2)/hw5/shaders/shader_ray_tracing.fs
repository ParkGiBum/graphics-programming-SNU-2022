#version 330
out vec4 FragColor;

in vec2 TexCoords;


// You can change the code whatever you want


const int MAX_DEPTH = 4; // maximum bounce
uniform samplerCube environmentMap;


struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    // phong shading coefficients
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;

    // reflect / refract
    vec3 R0; // Schlick approximation
    float ior; // index of refration

    // for refractive material
    vec3 extinction_constant;
    vec3 shadow_attenuation_constant;

    

    // 0 : phong
    // 1 : refractive dielectric
    // add more
    int material_type;
};

const int material_type_phong = 0;
const int material_type_refractive = 1;

// Just consider point light
struct Light{
    vec3 position;
    vec3 color;
    bool castShadow;
};
uniform vec3 ambientLightColor;

// hit information
struct HitRecord{
    float t;        // distance to hit point
    vec3 p;         // hit point
    vec3 normal;    // hit point normal
    Material mat;   // hit point material
};

// Geometry
struct Sphere {
    vec3 center;
    float radius;
    Material mat;
};

struct Plane {
    vec3 normal;
    vec3 p0;
    Material mat;
};

struct Box {
    vec3 box_min;
    vec3 box_max;
    Material mat;
};

struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
    // we didn't add material to triangle because it requires so many uniform memory when we use mesh...
};


const int mat_phong = 0;
const int mat_refractive = 1;

uniform Material material_ground;
uniform Material material_box;
uniform Material material_gold;
uniform Material material_dielectric_glass;
uniform Material material_mirror;
uniform Material material_lambert;
uniform Material material_mesh;


Sphere spheres[] = Sphere[](
  Sphere(vec3(1,0.5,-1), 0.5, material_gold),
  Sphere(vec3(-1,0.5,-1), 0.5, material_gold),
  Sphere(vec3(0,0.5,1), 0.5, material_lambert),
  Sphere(vec3(1,0.5,0), 0.5, material_lambert)
);

Box boxes[] = Box[](
  //Box(vec3(0,0,0), vec3(0.5,1,0.5), dielectric),
  Box(vec3(2,0,-3), vec3(3,1,-2), material_box)
);

Plane groundPlane = Plane(vec3(0,1,0), vec3(0,0,0), material_ground);
Triangle mirrorTriangle = Triangle( vec3(-3,0,0), vec3(0,0,-4), vec3(-1, 4, -2));

Light lights[] = Light[](
    Light(vec3(3,5,3), vec3(1,1,1), true),
    Light(vec3(-3,5,3), vec3(0.5,0,0), false),
    Light(vec3(-3,5,-3), vec3(0,0.5,0), false),
    Light(vec3(3,5,-3), vec3(0,0,0.5), false)
);

// use this for mesh
/*
layout (std140) uniform mesh_vertices_ubo
{
    vec3 mesh_vertices[500];
};

layout (std140) uniform mesh_tri_indices_ubo
{
    ivec3 mesh_tri_indices[500];
};

uniform int meshTriangleNumber;*/

// Math functions
/* returns a varying number between 0 and 1 */
float rand(vec2 co) {
  return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float max3 (vec3 v) {
  return max (max (v.x, v.y), v.z);
}

float min3 (vec3 v) {
  return min (min (v.x, v.y), v.z);
}


uniform vec3 cameraPosition;
uniform mat3 cameraToWorldRotMatrix;
uniform float fovY; //set to 45
uniform float H;
uniform float W;

Ray getRay(vec2 uv){
    // TODO:
    //vec3 rd = camera_lens_radius * random_in_unit_disk(uv);
    //vec3 offset = vec3(uv[0] * rd.x, uv[1] * rd.y, 0);
    float fovX = 2 * atan(tan(fovY * 0.5) * W/H);
    return Ray(cameraPosition,    cameraToWorldRotMatrix * vec3(- (uv[0] - 0.5) * W ,  (uv[1] - 0.5) * H , 550.0)    - cameraPosition );


}


const float bias = 0.0001; // to prevent point too close to surface.

vec3 point_at_parameter(Ray r,float t) {
  return r.origin + t * r.direction;
}

bool sphereIntersect(Sphere sp, Ray r, inout HitRecord hit){
    // TODO:
    float t_max = 100000000000000000000.0;
    float t_min = 0.001;
     vec3 oc = r.origin - sp.center;
     float a = dot(r.direction, r.direction);
     float b = dot(oc, r.direction);
     float c = dot(oc, oc) - sp.radius * sp.radius;
     float discriminant = b*b - a*c;
     if (discriminant > 0) {
       float temp = (-b - sqrt(b*b-a*c)) /a;
       if (temp < t_max && temp > t_min) {
         hit.t = temp;
         hit.p = point_at_parameter(r, hit.t);
         hit.normal = (hit.p - sp.center) / sp.radius;
         hit.mat = sp.mat;
         return true;
       }
       temp = (-b + sqrt(b*b-a*c)) /a;
       if (temp < t_max && temp > t_min) {
         hit.t = temp;
         hit.p = point_at_parameter(r, hit.t);
         hit.normal = (hit.p - sp.center) / sp.radius;
         hit.mat = sp.mat;
         return true;
       }
     }
     return false;

}

bool planeIntersect(Plane p, Ray r, inout HitRecord hit){
    // TODO:
    float t_max = 10000000000.0;
    float t_min = 0.0001;
    float t = (- 0.0 - r.origin.y) / r.direction.y;
    if (t < t_min || t > t_max) return false;
    hit.t = t;
    hit.p = point_at_parameter(r, t);
    hit.mat = p.mat;
    //hit.mat.Kd *= 0.0001;
    hit.normal = vec3(0, 1, 0);
    return true;
    
}


bool triangleIntersect(Triangle tri, Ray r, inout HitRecord hit){
    // TODO:
    float t_max = 10000000000.0;
    float t_min = 0.0001;

    vec3 e3 = tri.v1 - tri.v0; 
    vec3 e4 = tri.v2 - tri.v0; 
    vec3 N = cross(e3, e4);  

    float n4 = dot(N, r.direction); 
    float t = -(dot(N, r.origin) -dot(N, tri.v0)) / n4; 
    vec3 P = r.origin + t * r.direction; 

    vec3 C; 
    vec3 e0 = tri.v1 -  tri.v0; 
    vec3 vp0 = P     -  tri.v0; 
    vec3 e1 = tri.v2 -  tri.v1; 
    vec3 vp1 = P -      tri.v1; 
    vec3 e2 = tri.v0 -  tri.v2; 
    vec3 vp2 = P -      tri.v2; 
    hit.t = t;
    hit.p = P;
    hit.normal = N;

    C = cross(e0, vp0); 
    if (dot(N, C) < 0) return false; 
    C = cross(e1, vp1); 
    if (dot(N, C) < 0)  return false;  
    C = cross(e2, vp2); 
    if (dot(N, C) < 0) return false;  

    if (t < 0) return false; 
    if (t < t_min || t > t_max) return false;
    return true;
    
}


bool boxIntersect(Box b, Ray r, inout HitRecord hit){
    // TODO:
    vec3 c1;vec3 c2;vec3 c3;vec3 c4;
    vec3 c5;vec3 c6;vec3 c7;vec3 c8;

    vec3 med = b.box_min + b.box_max;
    med = med/2;
    c1.xyz = vec3(b.box_min.x, b.box_min.y, b.box_min.z);

    c2.xyz = vec3(b.box_min.x, b.box_min.y, b.box_max.z);
    c3.xyz = vec3(b.box_min.x, b.box_max.y, b.box_min.z);
    c4.xyz = vec3(b.box_max.x, b.box_min.y, b.box_min.z);

    c5.xyz = vec3(b.box_min.x, b.box_max.y, b.box_max.z);
    c6.xyz = vec3(b.box_max.x, b.box_min.y, b.box_max.z);
    c7.xyz = vec3(b.box_max.x, b.box_max.y, b.box_min.z);

    c8.xyz = vec3(b.box_max.x, b.box_max.y, b.box_max.z);

    Triangle tlist[12] = Triangle[12](Triangle(c1,c2,c3),Triangle(c1,c2,c4),Triangle(c1,c4,c3),
                        Triangle(c2,c3,c5),Triangle(c6,c2,c4),Triangle(c7,c4,c3),
                        Triangle(c5,c7,c3),Triangle(c6,c7,c4),Triangle(c2,c6,c5),
                        Triangle(c8,c7,c6),Triangle(c8,c7,c5),Triangle(c8,c5,c6));
    bool k = false;
    bool ever = false;
    int index = 0;
    float min = 1000000000;
    for (int i = 0; i < 12; i++) {
        k = triangleIntersect(tlist[i],r,hit);
        if(k){
            ever = true;
            if(min > hit.t){
                min = hit.t;
                index = i;
           
            }

        }
    }
    
    if(ever){
        triangleIntersect(tlist[index],r,hit);
        hit.mat = b.mat;
        //hit.mat.Kd *= 0.001;
        return true;
    }else{
        return false;
    }



}

float schlick(float cosine, float r0,int material_type) {
    // TODO:
    if(material_type != 2){return r0;}
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

vec3 schlick(float cosine, vec3 r0,int material_type) {
    // TODO:
    //if(material_type != 2){return r0;}
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 1);
    

}


bool trace(Ray r, out HitRecord hit){
    // TODO: trace single ray.
    HitRecord temp_hit;
    bool hit_anything = false;
    float closest_so_far = 10000000;


    for (int i = 0; i < spheres.length(); i++) {
        if (sphereIntersect(spheres[i], r, temp_hit)) {
          hit_anything = true;
          if(closest_so_far>temp_hit.t){
              hit = temp_hit;
              closest_so_far = temp_hit.t;
          }
        }
      }

    for (int i = 0; i < boxes.length(); i++) {
        if (boxIntersect(boxes[i], r, temp_hit)) {
            hit_anything = true;
            if(closest_so_far>temp_hit.t){
                hit = temp_hit;
                closest_so_far = temp_hit.t;
                //hit.mat = material_box;
            }
          }
     }
     if ( triangleIntersect(mirrorTriangle, r, temp_hit) ) {
        hit_anything = true;
        if(closest_so_far>temp_hit.t){
            hit = temp_hit;
            hit.mat = material_mirror;
            //hit.mat.Kd *= 0.001;
            closest_so_far = temp_hit.t;
        }
        
    }
    
    if (planeIntersect(groundPlane, r, temp_hit)) {
        hit_anything = true;
        if(closest_so_far>temp_hit.t){
            hit = temp_hit;
            closest_so_far = temp_hit.t;
        }
        
    }


    return hit_anything;

    

}

vec3 castRay(Ray ray) {
    // TODO: trace ray in iterative way.
    HitRecord hit;
    HitRecord tmp_hit;HitRecord tmp_hit2;
    vec3 col ;//= vec3(0.0, 0.0, 0.0);
    vec3 col2 ;
    vec3 total_attenuation = vec3(1.0, 1.0, 1.0);
    vec3 local_attenuation = vec3(1.0, 1.0, 1.0);
    
    for (int bounce = 0; bounce < 4; bounce++) {
        //col = vec3(0.0, 0.0, 0.0);
        col2 = vec3(0.0, 0.0, 0.0);
        local_attenuation = vec3(0.0,0.0,0.0);

        if(trace(ray,hit)){
            float cosine =  - dot(ray.direction, hit.normal) / length(ray.direction);
            for(int li = 0 ; li < lights.length(); li ++){
                Ray tmp_ray ;//= Ray(lights[li].position, hit.p - lights[li].position);
                tmp_ray.origin = lights[li].position; 
                tmp_ray.direction = normalize(hit.p - lights[li].position);
                vec3 l_col = lights[li].color;
                trace(tmp_ray,tmp_hit);


                Ray tmp_ray2 ;//= Ray(lights[li].position, hit.p - lights[li].position);
                tmp_ray2.origin = hit.p; 
                tmp_ray2.direction = - normalize(hit.p - lights[li].position);

                if(trace(tmp_ray,tmp_hit) ){
                    
                    vec3 ambient = hit.mat.Ka * l_col * 0.3;
                    if(ambient[0]<0.0) {ambient[0]=0.0;}
                    if(ambient[1]<0.0) {ambient[1]=0.0;}
                    if(ambient[2]<0.0) {ambient[2]=0.0;}

                    vec3 norm = normalize(hit.normal);
                    vec3 lightDir = tmp_ray.direction * -1;
                    float diff = max(dot(norm, normalize(lightDir)), 0.0);
                    vec3 diffuse = hit.mat.Kd * diff * l_col  ; 

                        
                    vec3 viewDir = - normalize(ray.origin - hit.p);
                    vec3 reflectDir = reflect(lightDir, hit.normal);  
                    float spec = pow(max(dot(viewDir, normalize(reflectDir)), 0.0), hit.mat.shininess + 1);
                    vec3 specular = l_col * spec * hit.mat.Ks; 
                    float sh = 1.0;
                    if(trace(tmp_ray2,tmp_hit2) && li == 0 ){
                        sh = 0.0;
                    }

                    vec3 col_t = ambient + (diffuse + specular) * sh ;
                    col2 += col_t * total_attenuation * (vec3(1.0,1.0,1.0) - schlick(cosine, hit.mat.R0, hit.mat.material_type));
                    col += col_t * total_attenuation * (vec3(1.0,1.0,1.0) - schlick(cosine, hit.mat.R0, hit.mat.material_type));///exp((tmp_hit.t + hit.t)/10.0);
                    
                   }
            }

            total_attenuation *= schlick(cosine,hit.mat.R0,hit.mat.material_type) ;
            ray.origin = hit.p;
            vec3 lightDir = ray.direction ;
            vec3 reflectDir = reflect(lightDir, hit.normal);
            ray.direction = normalize(reflectDir);

            
         }
         else{
            //col += vec3(0.5, 0.7, 1.0) * total_attenuation;

            vec3 tmp = ray.direction;
            col += ( texture(environmentMap, tmp.xyz) ).xyz * total_attenuation;
            
            break;
         }
        

    }

    return col;
}

void main()
{
    // TODO:

    const int nsamples = 1;
    vec3 color = vec3(0);
    Ray r = getRay(TexCoords);
    color += castRay(r);
    color /= nsamples;
    FragColor = vec4(color, 1.0);
}
