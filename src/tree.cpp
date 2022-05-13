#include "tree.hpp"

using namespace cgp;


mesh create_cylinder_mesh(float radius, float height)
{
    mesh cylinder; 

    // Number of samples
    int N = 20;

    // Geometry
    for(int k=0; k<N; ++k)
    {
        float u = k/float(N);
        vec3 p = {radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), 0.0f};
        cylinder.position.push_back( p );
        cylinder.position.push_back( p+vec3(0,0,height) );
    }

    // Connectivity
    for(int k=0; k<N; ++k)
    {
        int u00 = 2*k;
        int u01 = (2*k+1)%(2*N);
        int u10 = (2*(k+1))%(2*N);
        int u11 = (2*(k+1)+1) % (2*N);

        uint3 t1 = {u00, u10, u11};
        uint3 t2 = {u00, u11, u01};
        cylinder.connectivity.push_back(t1);
        cylinder.connectivity.push_back(t2);
    }

    cylinder.fill_empty_field();

    return cylinder;
}

mesh create_cone_mesh(float radius, float height, float z_offset)
{
   mesh m;

    // conical structure
    // *************************** //

    int N = 20;

    // geometry
    for(int k=0; k<N; ++k)
    {
        float u = k/float(N);
        vec3 p = {radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), 0.0f};
        m.position.push_back( p+vec3{0,0,z_offset} );
    }
    // apex
    m.position.push_back({0,0,height+z_offset});

    // connectivity
    int Ns = N;
    for(int k=0; k<Ns; ++k) {
        m.connectivity.push_back( {k , (k+1)%Ns, Ns} );
    }

    // close the bottom of the cone
    // *************************** //

    // Geometry
    for(int k=0; k<N; ++k)
    {
        float u = k/float(N);
        vec3 p = {radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), 0.0f};
        m.position.push_back( p+vec3{0,0,z_offset} );
    }
    // central position
    m.position.push_back( {0,0,z_offset} );

    // connectivity
    for(int k=0; k<Ns; ++k)
        m.connectivity.push_back( {k+Ns+1, (k+1)%Ns+Ns+1, 2*Ns+1} );

    m.fill_empty_field();
    return m;
}

mesh create_tree()
{

    float h = 0.7f; // trunk height
    float r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_cylinder_mesh(r, h);
    trunk.color.fill({0.4f, 0.3f, 0.3f});


    // Create a green foliage from 3 cones
    mesh foliage = create_cone_mesh(4*r, 6*r, 0.0f);      // base-cone
    foliage.push_back(create_cone_mesh(4*r, 6*r, 2*r));   // middle-cone
    foliage.push_back(create_cone_mesh(4*r, 6*r, 4*r));   // top-cone
    foliage.position += vec3(0,0,h);                 // place foliage at the top of the trunk
    foliage.color.fill({0.4f, 0.6f, 0.3f});
       
    // The tree is composed of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}

