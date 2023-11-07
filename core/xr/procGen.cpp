#include "../ew/ewMath/ewMath.h"
#include "procGen.h"

ew::MeshData xr::createSphere(float radius, int numSegments) {
    ew::MeshData mesh;
    float thetaStep = ew::TAU / numSegments, phiStep = ew::PI / numSegments, phi, theta;
    int row, col, i, poleStart, sideStart;

    for (row = 0; row <= numSegments; row++) {
        phi = row * phiStep;
        for (col = 0; col <= numSegments; col++) {
            theta = col * thetaStep;
            ew::Vertex vertex;

            vertex.pos.x = radius * cos(theta) * sin(phi);
            vertex.pos.y = radius * cos(phi);
            vertex.pos.z = radius * sin(theta) * sin(phi);

            vertex.normal = ew::Normalize(vertex.pos - ew::Vec3(0, 0, 0));

            vertex.uv.x = theta / ew::TAU;
            vertex.uv.y = 1 - (phi / ew::PI);

            mesh.vertices.push_back(vertex);
        }
    }

    poleStart = 0;
    sideStart = numSegments + 1;

    for (i = 0; i < numSegments; i++) {
        mesh.indices.push_back(sideStart + i);
        mesh.indices.push_back(poleStart + i);
        mesh.indices.push_back(sideStart + i + 1);
    }

    int columns = numSegments + 1, start;
    for (row = 1; row < numSegments - 1; row++) {
        for (col = 0; col < numSegments; col++) {
            start = row * columns + col;

            mesh.indices.push_back(start);
            mesh.indices.push_back(start + 1);
            mesh.indices.push_back(start + columns);
            mesh.indices.push_back(start + columns);
            mesh.indices.push_back(start + 1);
            mesh.indices.push_back(start + columns + 1);
        }
    }

    poleStart = mesh.vertices.size() - numSegments - 1;
    sideStart = poleStart - numSegments - 1;

    for (i = 0; i < numSegments; i++) {
        mesh.indices.push_back(sideStart + i);
        mesh.indices.push_back(sideStart + i + 1);
        mesh.indices.push_back(poleStart + i);
    }

    return mesh;
}

ew::MeshData xr::createCylinder(float height, float radius, int numSegments) {
    ew::MeshData mesh;

    float topHeight = height / 2, bottomHeight = -topHeight;
    float thetaStep = ew::TAU / numSegments, theta;
    float start = 1, center = 0, sideStart;
    int i, columns;

    mesh.vertices.push_back({ ew::Vec3(0, topHeight, 0), ew::Vec3(0.0, 1.0, 0.0), ew::Vec2(0.5f, 0.5f) });

    // TOP CAP RING
    for (i = 0; i <= numSegments; i++) {
        theta = i * thetaStep;
        ew::Vertex vertex;
        vertex.pos = ew::Vec3(cos(theta) * radius, topHeight, sin(theta) * radius);
        vertex.normal = ew::Vec3(0.0, 1.0, 0.0);
        vertex.uv = (ew::Vec2(cos(theta), sin(theta)) + 1) / 2;
        mesh.vertices.push_back(vertex);
    }

    // TOP SIDE RING
    for (i = 0; i <= numSegments; i++) {
        theta = i * thetaStep;
        ew::Vertex vertex;
        vertex.pos = ew::Vec3(cos(theta) * radius, topHeight, sin(theta) * radius);
        vertex.normal = ew::Vec3(cos(theta), 0, sin(theta));
        vertex.uv = ew::Vec2(i / (float)numSegments, 1);
        mesh.vertices.push_back(vertex);
    }

    // BOTTOM SIDE RING
    for (i = 0; i <= numSegments; i++) {
        theta = i * thetaStep;
        ew::Vertex vertex;
        vertex.pos = ew::Vec3(cos(theta) * radius, bottomHeight, sin(theta) * radius);
        vertex.normal = ew::Vec3(cos(theta), 0, sin(theta));
        vertex.uv = ew::Vec2(i / (float)numSegments, 0);
        mesh.vertices.push_back(vertex);
    }

    // BOTTOM CAP RING
    for (i = 0; i <= numSegments; i++) {
        theta = i * thetaStep;
        ew::Vertex vertex;
        vertex.pos = ew::Vec3(cos(theta) * radius, bottomHeight, sin(theta) * radius);
        vertex.normal = ew::Vec3(0.0, -1.0, 0.0);
        vertex.uv = ew::Vec2(cos(theta), sin(theta));
        mesh.vertices.push_back(vertex);
    }

    mesh.vertices.push_back({ ew::Vec3(0, bottomHeight, 0), ew::Vec3(0.0, 0.0, 0.0) });

    start = 1;
    center = 0;
    for (i = 0; i < numSegments; i++) {
        mesh.indices.push_back(start + i);
        mesh.indices.push_back(center);
        mesh.indices.push_back(start + i + 1);
    }

    sideStart = numSegments + 1;
    columns = numSegments + 1;

    for (i = 0; i < columns; i++) {
        start = sideStart + i;
        mesh.indices.push_back(start);
        mesh.indices.push_back(start + 1);
        mesh.indices.push_back(start + columns);
        mesh.indices.push_back(start + 1);
        mesh.indices.push_back(start + columns + 1);
        mesh.indices.push_back(start + columns);
    }

    center = mesh.vertices.size() - 1;
    start = center - columns;

    for (i = 0; i < numSegments; i++) {
        mesh.indices.push_back(start + i + 1);
        mesh.indices.push_back(center);
        mesh.indices.push_back(start + i);
    }

    return mesh;
}

ew::MeshData xr::createPlane(float size, int subdivisions) {
    ew::MeshData mesh;
    int row, col, columns = subdivisions + 1, start;

    for (row = 0; row <= subdivisions; row++) {
        for (col = 0; col <= subdivisions; col++) {
            ew::Vertex vertex;

            vertex.pos.x = size * static_cast<float>(col) / subdivisions;
            vertex.pos.z = -size * static_cast<float>(row) / subdivisions;

            vertex.uv.x = (static_cast<float>(col) / subdivisions);
            vertex.uv.y = (static_cast<float>(row) / subdivisions);

            vertex.normal = ew::Vec3(0.0, 1.0, 0.0);
            mesh.vertices.push_back(vertex);
        }
    }

    for (row = 0; row < subdivisions; row++) {
        for (col = 0; col < subdivisions; col++) {
            start = row * columns + col;

            mesh.indices.push_back(start);
            mesh.indices.push_back(start + 1);
            mesh.indices.push_back(start + columns + 1);

            mesh.indices.push_back(start + columns + 1);
            mesh.indices.push_back(start + columns);
            mesh.indices.push_back(start);
        }
    }

    return mesh;
}