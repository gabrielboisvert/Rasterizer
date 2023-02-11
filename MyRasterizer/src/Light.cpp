#include "../include/Light.h"
#include "../include/Mat3/Mat3.h"

Light::Light(lm::vec3 position, float ambiantComponent, float diffuseComponent, float specularComponent) : 
	position(position), 
	ambiantComponent(ambiantComponent), 
	diffuseComponent(diffuseComponent), 
	specularComponent(specularComponent) {}

lm::vec3& Light::getPosition()
{
	return this->position;
}

float Light::getAmbiant()
{
	return this->ambiantComponent;
}

float Light::getDiffuse()
{
	return this->diffuseComponent;
}

float Light::getSpecular()
{
	return this->specularComponent;
}

Color Light::calculateColor(const lm::vec3& normal, Color& color, lm::mat4& transform)
{
	lm::vec3 objectColor(color.getRedValue(), color.getGreenValue(), color.getBlueValue());

	//Ambiant light
	lm::vec3 ambiant = this->ambiantComponent * this->ambiantColor;
	
	//Diffuse light
	lm::vec3 diffuse = this->diffuseComponent * this->diffuseColor;
	lm::mat3 model(transform);
	lm::vec3 norm = (model.inverse().transpose() * normal.normalized()).normalized();	
	float diff = std::max(0.f, (norm.dotProduct(this->direction)));
	lm::vec3 duffuseCalc = (diff * diffuse);

	//Specular light
	lm::vec3 reflectDir = (-this->direction).reflexion(norm);
	float spec = std::pow(std::max(0.f, norm.dotProduct(reflectDir)), this->shininess);
	lm::vec3 specularCalc = (this->specularComponent * spec) * this->specularColor;

	lm::vec3 result = (duffuseCalc + ambiant + specularCalc).scale(objectColor);
	return Color(unsigned char(result.X()), unsigned char(result.Y()), unsigned char(result.Z()), unsigned char(color.getAlphaValue()));
}

lm::vec3 Light::getLightReflection(lm::vec3 v0, lm::vec3 v1)
{
	lm::vec3 negatedP0 = v0 * -1;
	float dotProductResult = negatedP0.dotProduct(v1);

	return { negatedP0 - (v1 * (2 * dotProductResult)) };
}
