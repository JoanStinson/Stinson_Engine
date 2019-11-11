#include "Mesh.h"
#include "Globals.h"
#include <glew.h>
#include <Importer.hpp>
#include <postprocess.h>
#include <scene.h>

Mesh::Mesh(const char *filename, unsigned texture, unsigned program) : texture(texture), program(program) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filename, 0);
	if (scene == nullptr) {
		LOG("Unable to load mesh: %s\n", importer.GetErrorString());
	}
	else {
		for (int i = 0; i < scene->mNumMeshes; ++i) 
			meshEntries.push_back(new Mesh::Vertex(scene->mMeshes[i]));
	}
}

Mesh::~Mesh() {
	for (int i = 0; i < meshEntries.size(); ++i) 
		delete meshEntries[i];
	meshEntries.clear();
}

void Mesh::Render() {
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	for (int i = 0; i < meshEntries.size(); ++i) 
		meshEntries[i]->Render();
}

void Mesh::Render(unsigned meshTexture, unsigned meshProgram) {
	glUseProgram(meshProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, meshTexture);
	glUniform1i(glGetUniformLocation(meshProgram, "texture0"), 0);

	for (int i = 0; i < meshEntries.size(); ++i)
		meshEntries[i]->Render();
}

Mesh::Vertex::Vertex(aiMesh *mesh) {
	vbo[(int)BUFFER::VERTEX] = 0;
	vbo[(int)BUFFER::TEXCOORD] = 0;
	vbo[(int)BUFFER::NORMAL] = 0;
	vbo[(int)BUFFER::INDEX] = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	elementCount = mesh->mNumFaces * 3;

	if (mesh->HasPositions()) {
		float *vertices = new float[mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;
		}

		glGenBuffers(1, &vbo[(int)BUFFER::VERTEX]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[(int)BUFFER::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		delete[] vertices;
	}

	if (mesh->HasTextureCoords(0)) {
		float *texCoords = new float[mesh->mNumVertices * 2];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &vbo[(int)BUFFER::TEXCOORD]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[(int)BUFFER::TEXCOORD]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		delete[] texCoords;
	}

	if (mesh->HasNormals()) {
		float *normals = new float[mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &vbo[(int)BUFFER::NORMAL]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[(int)BUFFER::NORMAL]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		delete[] normals;
	}

	if (mesh->HasFaces()) {
		unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
		for (int i = 0; i < mesh->mNumFaces; ++i) {
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &vbo[(int)BUFFER::INDEX]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[(int)BUFFER::INDEX]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);

		delete[] indices;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Vertex::~Vertex() {
	if (vbo[(int)BUFFER::VERTEX]) {
		glDeleteBuffers(1, &vbo[(int)BUFFER::VERTEX]);
	}
	if (vbo[(int)BUFFER::TEXCOORD]) {
		glDeleteBuffers(1, &vbo[(int)BUFFER::TEXCOORD]);
	}
	if (vbo[(int)BUFFER::NORMAL]) {
		glDeleteBuffers(1, &vbo[(int)BUFFER::NORMAL]);
	}
	if (vbo[(int)BUFFER::INDEX]) {
		glDeleteBuffers(1, &vbo[(int)BUFFER::INDEX]);
	}
	glDeleteVertexArrays(1, &vao);
}

void Mesh::Vertex::Render() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}