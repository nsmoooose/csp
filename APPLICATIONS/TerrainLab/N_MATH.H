
typedef struct vector3 {
    float x;
    float y;
    float z;
public:
    vector3() { }
    vector3(float f);
    vector3(float _x, float _y, float _z);
    vector3(const float f[3]);
    // =====================================
    // Access grants
    // =====================================
    const float&operator[](int i) const;
    float&operator[](int i);
    // =====================================
    // Assignment operators
    // =====================================
    vector3& operator += (const vector3& v);
    vector3& operator -= (const vector3& v);
    vector3& operator *= (const vector3& v);
    vector3& operator /= (const vector3& v);
    vector3& operator *= (float s);
    vector3& operator /= (float s);
    // =====================================
    // Unary operators
    // =====================================
    friend vector3 operator + (const vector3& v);
    friend vector3 operator - (const vector3& v);
    // =====================================
    // Binary operators
    // =====================================
    // Addition and subtraction
    friend vector3 operator + (const vector3& v1, const vector3& v2);
    friend vector3 operator - (const vector3& v1, const vector3& v2);
    // Scalar multiplication and division
    friend vector3 operator * (const vector3& v, float s);
    friend vector3 operator * (float s, const vector3& v);
    friend vector3 operator / (const vector3& v, float s);
    // Memberwise multiplication and division
    friend vector3 operator * (const vector3& v1, const vector3& v2);
    friend vector3 operator / (const vector3& v1, const vector3& v2);
    // Vector dominance
    friend int operator < (const vector3& v1, const vector3& v2);
    friend int operator <= (const vector3& v1, const vector3& v2);
    // Bitwise equality
    friend int operator == (const vector3& v1, const vector3& v2);
    // Length-related functions
    friend float SquareMagnitude (const vector3& v);
    friend float Magnitude (const vector3& v);
    // Returns vector with same direction and unit length
    friend vector3 Normalize (const vector3& v);
    // Return min/max component of the input vector
    friend float Min (const vector3& v);
    friend float Max (const vector3& v);
    // Return memberwise min/max of input vectors
    friend vector3 Minimize (const vector3& v1, const vector3& v2);
    friend vector3 Maximize (const vector3& v1, const vector3& v2);
    // Dot and cross product
    friend float DotProduct (const vector3& v1, const vector3& v2);
    friend vector3 CrossProduct (const vector3& v1, const vector3& v2);
} _vector3;

typedef struct{
    vector3 origin;
    vector3 direction;
}ray;

///////////// VECTOR3;

inline vector3::vector3(float f){x=y=z=f;};
inline vector3::vector3(float _x,float _y,float _z){x=_x;y=_y;z=_z;};
inline vector3::vector3(const float f[3]){x=f[0];y=f[1];z=f[2];};
inline const float& vector3::operator[](int i) const{return (&x)[i];};
inline float& vector3::operator[](int i){return (&x)[i];};
inline vector3& vector3::operator += (const vector3& v){x+=v.x;y+=v.y;z+=v.z;return *this;};
inline vector3& vector3::operator -= (const vector3& v){x-=v.x;y-=v.y;z-=v.z;return *this;};
inline vector3& vector3::operator *= (const vector3& v){x*=v.x;y*=v.y;z*=v.z;return *this;};
inline vector3& vector3::operator /= (const vector3& v){x/=v.x;y/=v.y;z/=v.z;return *this;};
inline vector3& vector3::operator *= (float s){x*=s;y*=s;z*=s;return *this;};
inline vector3& vector3::operator /= (float s){x/=s;y/=s;z/=s;return *this;};
inline vector3 operator + (const vector3& v){return v;};
inline vector3 operator - (const vector3& v){return vector3(-v.x,-v.y,-v.z);};
inline vector3 operator + (const vector3& v1,const vector3& v2){return vector3(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);};
inline vector3 operator - (const vector3& v1,const vector3& v2){return vector3(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);};
inline vector3 operator * (const vector3& v1,const vector3& v2){return vector3(v1.x*v2.x,v1.y*v2.y,v1.z*v2.z);};
inline vector3 operator / (const vector3& v1,const vector3& v2){return vector3(v1.x/v2.x,v1.y/v2.y,v1.z/v2.z);};
inline int operator < (const vector3& v1,const vector3& v2){return v1[0]<v2[0]&&v1[1]<v2[1]&&v1[2]<v2[2];};
inline int operator <= (const vector3& v1,const vector3& v2){return v1[0]<=v2[0]&&v1[1]<=v2[1]&&v1[2]<=v2[2];};
inline vector3 operator * (const vector3& v,float s){return vector3(s*v.x,s*v.y,s*v.z);};
inline vector3 operator * (float s,const vector3& v){return vector3(s*v.x,s*v.y,s*v.z);};
inline vector3 operator / (const vector3& v,float s){return vector3(v.x/s,v.y/s,v.z/s);};
inline int operator == (const vector3& v1,const vector3& v2){return v1.x==v2.x&&v1.y==v2.y&&v1.z==v2.z;};
inline float Magnitude(const vector3& v){return (float)sqrt(SquareMagnitude(v));};
inline float SquareMagnitude(const vector3& v){return v.x*v.x+v.y*v.y+v.z*v.z;};
inline vector3 Normalize(const vector3& v){return v/Magnitude(v);};
inline float Min(const vector3& v)
{
   float ret = v.x;
   if (v.y < ret) ret = v.y;
   if (v.z < ret) ret = v.z;
   return ret;
};
inline float Max(const vector3& v)
{
   float ret = v.x;
   if (ret < v.y) ret = v.y;
   if (ret < v.z) ret = v.z;
   return ret;
};
inline vector3 Minimize(const vector3& v1,const vector3& v2){return vector3(v1[0]<v2[0]?v1[0]:v2[0],v1[1]<v2[1]?v1[1]:v2[1],v1[2]<v2[2]?v1[2]:v2[2]);}
inline vector3 Maximize(const vector3& v1,const vector3& v2){return vector3(v1[0]>v2[0]?v1[0]:v2[0],v1[1]>v2[1]?v1[1]:v2[1],v1[2]>v2[2]?v1[2]:v2[2]);}
inline float DotProduct(const vector3& v1,const vector3& v2)
{
        return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
};
inline vector3 CrossProduct(const vector3& v1,const vector3& v2)
{
        vector3 result;
        result[0] = v1[1] * v2[2] - v1[2] * v2[1];
        result[1] = v1[2] * v2[0] - v1[0] * v2[2];
        result[2] = v1[0] * v2[1] - v1[1] * v2[0];
        return result;
};
