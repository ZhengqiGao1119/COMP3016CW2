# COMP3016CW2
## Assignment Report

### Running the Program

To run the program in a Windows environment, simply double-click on the executable file car.exe. 

**Interaction Operations:**

- Press keys w, s, a, d: Control the camera to move forward, backward, left, and right.

- Mouse: Control the direction of the camera's view.

- Press key j: Move the car forward.

- Press key l: Move the car backward.

- Press key m: Turn on the car's headlights.

- Press key n: Turn off the car's headlights.

### Code Explanation

#### 1.basic object geometry

---

**Class Mesh**

​	Mesh is a base class representing a model. All model classes need to inherit from Mesh and implement their own Render function

```c++
virtual void Render() = 0;
```

**Class Quad**

​	Quad is a basic primitive consisting of four vertices and two triangles, with a size of 1x1, parallel to the xoy plane/

quad vertices

```c++
float vertices[] = {
    -0.5f, -0.5f,  0.f,
    0.5f, -0.5f,  0.f,
    0.5f,  0.5f,  0.f,
    0.5f,  0.5f,  0.f,
    -0.5f,  0.5f,  0.f,
    -0.5f, -0.5f,  0.f,
};
```

quad texcoord：for a better representation of the field, you can adjust the number of times you need to copy the expanded texture according to the parameters tx, ty.

```c++
float textureCoords[] = {
    0.0f, 0.0f,
    tx, 0.0f,
    tx, ty,
    tx, ty,
    0.0f, ty,
    0.0f, 0.0f,
};
```

Data format: create a VAO and VBO [3] to store the data of vertices, normal vectors, texture coordinates

```c++
glGenBuffers(3, VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
glEnableVertexAttribArray(0);
glEnableVertexAttribArray(1);
glEnableVertexAttribArray(2);
```

Application: The grass, the road, and the yellow line in the scene are obtained using the Quad transform.

Class Cube

​		Cube is composed of 6 faces, its center is at the origin, and its size is 1x1x1. Each face is composed of two triangles.

```c++
float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,0.5f, -0.5f,  0.5f,0.5f,  0.5f,  0.5f,0.5f,  0.5f,  0.5f,-0.5f,  0.5f,  0.5f,-0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,-0.5f, -0.5f, -0.5f,-0.5f, -0.5f, -0.5f,-0.5f, -0.5f,  0.5f,-0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,0.5f,  0.5f, -0.5f,0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,0.5f, -0.5f,  0.5f,0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f,-0.5f, -0.5f,  0.5f,-0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,0.5f,  0.5f, -0.5f, 0.5f,  0.5f,  0.5f,0.5f,  0.5f,  0.5f,-0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
};
```

Application: The car body part of the scene is obtained using two cubes stacked together


**Class Sphere**

An arbitrary point (*x, y, z*) on a sphere can be computed by parametric equations with the corresponding sector angle *θ* and stack angle *ϕ*.
$$
x = (r \cdot cos \phi) \cdot cos\theta\\
y = (r \cdot cos \phi) \cdot sin\theta\\
z = (r \cdot sin \phi)
$$
The range of sector angles is from 0 to 360 degrees, and the stack angles are from 90 (top) to -90 degrees (bottom). The sector and stack angle for each step can be calculated by the following;
$$
\theta = 2\pi \cdot \frac{sectorStep}{sectorCount}\\
\phi = 2\pi - \pi\cdot\frac{stackStep}{stackCount}
$$
in code: i use two for loop to caluculate all of vertcies, normals and texture coords and store them into array. normals is vertices / radius.

```c++
for (int i = 0; i <= stackCount; ++i) {
    stackAngle = PI / 2 - i * stackStep;
    float xy = radius * cosf(stackAngle);             // r * cos(u)
    float z = radius * sinf(stackAngle);              // r * sin(u)
    for (int j = 0; j <= sectorCount; ++j) {
        sectorAngle = j * sectorStep;
		float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
		float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

        // vertex position (x, y, z)
        vertices[cnt * 3 + 0] = x;
        vertices[cnt * 3 + 1] = y;
        vertices[cnt * 3 + 2] = z;

        // normalized vertex normal (nx, ny, nz)
        normals[cnt * 3 + 0] = x / m_radius;
        normals[cnt * 3 + 1] = y / m_radius;
        normals[cnt * 3 + 2] = z / m_radius;

         // vertex tex coord (s, t) range between [0, 1]
        texCoords[cnt * 2 + 0] = (float)j / sectorCount;
        texCoords[cnt * 2 + 1] = (float)i / stackCount;
		cnt += 1;
	}
}
```

then caculate indices. think it as a quad map. stacks and sectors ,Divide the sphere into several quadrangles, and each quadrangle is composed of two triangles.

```c++
for (int i = 0; i < stackCount; ++i) {
    float k1 = i * (sectorCount + 1);     // beginning of current stack
    float k2 = k1 + sectorCount + 1;      // beginning of next stack
    for (int j = 0; j < sectorCount; ++j) {
        int k1 = (i + 0) * (sectorCount + 1) + j;
        int k2 = (i + 1) * (sectorCount + 1) + j;

        //k1 = > k2 = > k1 + 1
        indices[iCnt++] = k1;
        indices[iCnt++] = k2;
        indices[iCnt++] = k1 + 1;

        // k1+1 => k2 => k2+1
        indices[iCnt++] = k1 + 1;
        indices[iCnt++] = k2;
        indices[iCnt++] = k2 + 1;

    }
}
```

Application: The headlights in the scene are obtained by compressing the ball in the x-axis.

**Class Cylinder**

​	The definition of a cylinder is a 3D closed surface that has 2 parallel circular bases at the ends and connected by a curved surface (side). Suppose a cylinder is centered at the origin and its radius is *r* and the height is *h*. An arbitrary point (*x, y, z*) on the cylinder can be computed from the equation of circle with the corresponding sector angle *θ*.
$$
\theta = 2\pi \cdot \frac{sectorStep}{sectorCount}\\
x = r \cdot cos\theta \\
y = r \cdot sin\theta \\
z = \frac{h}{2} or \frac{-h}{2}
$$
in code, i calculate top radius and bottom radius positions with increase angle $\theta$， the normal of every points is
$$
normal.x = x / radius;
normal.y = y / radius;
normal.z = 0.f
$$

```c++
for (int i = 0; i < 2; ++i) {
    float z = zArray[i];
    for (int j = 0; j <= sectorCount; ++j) {
        sectorAngle = j * sectorStep;
        float x = m_radius * cos(sectorAngle);             // r * cos(v)
        float y = m_radius * sin(sectorAngle);             // r * sin(v)

        vertices[cnt * 3 + 0] = x;
        vertices[cnt * 3 + 1] = y;
        vertices[cnt * 3 + 2] = z;

        // normalized vertex normal (nx, ny, nz)
        normals[cnt * 3 + 0] = x / m_radius;
        normals[cnt * 3 + 1] = y / m_radius;
        normals[cnt * 3 + 2] = 0.f;

        // vertex tex coord (s, t) range between [0, 1]
        texCoords[cnt * 2 + 0] = (float)j / sectorCount;
        texCoords[cnt * 2 + 1] = z / m_height;


        cnt += 1;
    }
}
```

bottom and top is a circle.

```c++
for (int j = 0; j <= sectorCount; ++j) {
    sectorAngle = j * sectorStep;
    float x = m_radius * cos(sectorAngle);             // r * cos(v)
    float y = m_radius * sin(sectorAngle);             // r * sin(v)

    vertices[cnt * 3 + 0] = x;
    vertices[cnt * 3 + 1] = y;
    vertices[cnt * 3 + 2] = height;

    // normalized vertex normal (nx, ny, nz)
    normals[cnt * 3 + 0] = 0.f;
    normals[cnt * 3 + 1] = 0.f;
    normals[cnt * 3 + 2] = 1.f;

    // vertex tex coord (s, t) range between [0, 1]
    texCoords[cnt * 2 + 0] = -(x / m_radius) * 0.5f + 0.5f;
    texCoords[cnt * 2 + 1] = -(y / m_radius) * 0.5f + 0.5f;
    cnt += 1;
}
```

Application: The wheels in the scene are represented using cylinders.


#### 2.perspective projection

---

Projection Matrix

```c++
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
```

LookAt Matrix

```c++
glm::mat4 view = camera.GetViewMatrix();
```

#### 3. first-person style 3D camera

---

make a class named Camera, has some attribs;

```c++
// Camera Attributes
glm::vec3 Position;
glm::vec3 Front;
glm::vec3 Up;
glm::vec3 Right;
glm::vec3 WorldUp;
// Euler Angles
float Yaw;
float Pitch;
// Camera options
float MovementSpeed;
float MouseSensitivity;
float Zoom;
```

Positionis contrl the position of camera. Yawand Pitch are contrl camera direction.

first-person camera update 

```c++
void updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}
```

keyboard contrl.

```c++
if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
float span = 5.0 * deltaTime;
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, span);
if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, span);
if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, span);
if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, span);
```



#### 4.point light shading

---

​		A point light is a light source somewhere in the world that shines in all directions, but the light fades with distance. Imagine the light bulb and torch as projectors. They are both point light sources.

define a struct represent point light.

```c++
struct PointLight {
	Vec3f lightColor;
	Vec3f lightPos;
	Vec3f lightAmbient;
	Vec3f lightDiffuse;
	Vec3f lightSpecular;
	float lightConstant;
	float lightLinear;
	float lightQuadratic;
};
```

there are three ponts lights in the scene. for light 3, set the parametric of lights.

```c++
PointLight light[1];
light[0].lightColor = glm::vec3{ 1, 1, 1 };
light[0].lightPos = glm::vec3{ 4, 8, 5 };
light[0].lightAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
light[0].lightDiffuse = glm::vec3{ 1, 1, 1 };
light[0].lightSpecular = glm::vec3{ 1, 1, 1 };
light[0].lightConstant = 1.f;
light[0].lightLinear = 0.05;
light[0].lightQuadratic = 0;
```

use opengl uniform upload light parmetric

```c++
pCurShader->setVec3("pointLights[0].lightColor", light[0].lightColor);
pCurShader->setVec3("pointLights[0].lightPos", light[0].lightPos);
pCurShader->setVec3("pointLights[0].lightAmbient", light[0].lightAmbient);
pCurShader->setVec3("pointLights[0].lightDiffuse", light[0].lightDiffuse);
pCurShader->setVec3("pointLights[0].lightSpecular", light[0].lightSpecular);
pCurShader->setFloat("pointLights[0].lightConstant", light[0].lightConstant);
pCurShader->setFloat("pointLights[0].lightLinear", light[0].lightLinear);
pCurShader->setFloat("pointLights[0].lightQuadratic", light[0].lightQuadratic);
```

shader point light caulate

**ambinet**

```c++
vec3 ambient  = light.lightAmbient * material.matAmbient;
```

**diffuse**

![image-20221222211252329](report.assets/image-20221222211252329.png)

```c++
float diff = max(dot(normal, lightDir), 0.0);
vec3 diffuse  = light.lightColor * light.lightDiffuse * diff * material.matDiffuse;
```

**specular**

```c++
vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.matShininess);
specular * spec * material.matSpecular;   
```

**attenuation**

The gradual reduction of light intensity with the increase of light propagation distance is usually called Attenuation.
$$
F_{att}= \frac{1.0}{K_c+K_l*d + k_q * d^2}
$$

```c++
float distance = length(light.lightPos - fragPos);
float attenuation = 1.0 / (light.lightConstant + light.lightLinear * distance + light.lightQuadratic * (distance * distance));   
ambient  *= attenuation;
diffuse  *= attenuation;
specular *= attenuation;
```



#### 5.Spot light

A spotlight is a light source located at a certain location in the environment that shines light in one specific direction rather than all directions. The result is that only objects within a specific radius in the direction of the spotlight will be illuminated, all other objects will remain dark. In our program i use spotlighting to represent headlights.

In OpenGL, a spotlight is represented by a position in world space, a direction, and a cutoff angle, which specifies the radius of the spotlight (the radius of the cone, not the radius of the distance from the light source). For each clip, i calculate whether the clip lies between the cutoff directions of the spotlight (i.e. within the cone) or not.

The values i need in the fragment shader are the position vector of the spotlight (to calculate the direction vector of the light), the direction vector of the spotlight and a tangent angle. These are stored in the Light struct:

```c++
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;   
    bool lightOn;
};
```

Calculate the value of the angle $\theta$ between the LightDir vector and the SpotDir vector and compare it to the tangent angle $\phi$ of the spot radius to determine if it is inside the spot:

```c++
vec3 lightDir = normalize(light.position - fragPos);
float theta = dot(lightDir, normalize(-light.direction)); 
float epsilon = light.cutOff - light.outerCutOff;
float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
```

Trimming the range of lights

```c++
ambient *= attenuation * intensity;
diffuse *= attenuation * intensity;
specular *= attenuation * intensity;
```

The final result:

#### 6.materials

define a struct represent material

```c++
struct Material {
	Vec3f matAmbient;
	Vec3f matDiffuse;
	Vec3f matSpecular;
	float matShininess;
};
```

uniform upload

```c++
shader.setVec3("material.matAmbient", root->m_mat.matAmbient);
shader.setVec3("material.matDiffuse", root->m_mat.matDiffuse);
shader.setVec3("material.matSpecular", root->m_mat.matSpecular);
shader.setFloat("material.matShininess", root->m_mat.matShininess);
```

shader

```c++
vec3 ambient  = light.lightAmbient * material.matAmbient;
vec3 diffuse  = light.lightColor * light.lightDiffuse * diff * material.matDiffuse;
vec3 specular = light.lightColor * light.lightSpecular * spec * material.matSpecular;
```

Some of the material parameters set

**base**

```c++
Material matBase;
matBase.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
matBase.matDiffuse = glm::vec3{ 1.0f, 1.0f, 1.0f };
matBase.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
matBase.matShininess = 32.f;
```

yellow

```
Material matYellow;
matYellow.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
matYellow.matDiffuse = glm::vec3{ 1.0f, 1.0f, 0.0f };
matYellow.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
matYellow.matShininess = 32.f;
```

#### 7.Texturing map.

---

there are two texture in the scene. grass in the ground and road texuture.

load texture by stb_image library

```c++
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
```

bind texture

```c++
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texVec[0]);
```

shader use texture

```c++
uniform sampler2D texture1;
vec3 color = texture(texture1, TexCoord).rgb;
```



#### 8. Interaction

Movement of car

​	I set a variable carMove and change the value of the variable by pressing a key. Then i convert carMove to the translation matrix carModel

```c++
glm::mat4 carModel(1.f);
carModel = carModel * glm::translate(glm::mat4(1.f), glm::vec3(carMove, 0, 0));
```

Apply matrix carModel to all car components (including tires, body, lights, window lights)

```c++
model = glm::mat4(1.f);
model = model * carModel;
model = glm::translate(model, glm::vec3(-1.0, 0, -0.8));
model = glm::translate(model, glm::vec3(0, 0.3, -0.1));
model = glm::scale(model, glm::vec3(0.3, 0.3, 0.2));
```

**Lights on and off**

Controlled via the variable carLight. When carLight is true the light is turned on, false the light is turned off. carLight is transferred to the spotLight's lightOn variable

```c++
pCurShader->setBool("spotLight[0].lightOn", carLight);
```

Deciding whether or not to accumulate spotlights based on variables **lightOn** in shader

```c++
for(int i = 0; i < 1; i++) {
    if(spotLight[i].lightOn) {
        vec3 viewDirCar = normalize(carPos - FragPos); 
        result += CalcSpotLight(spotLight[i], norm, FragPos, viewDirCar);    
    }
}
```



## Signature

I create a texture containing the initials of the name **Zhengqi Gao**
It was then displayed in Quad and placed on top of the car

## Link github

https://github.com/ZhengqiGao1119/COMP3016CW2

## Link youtobe

https://youtu.be/q0JADjCMFK0
