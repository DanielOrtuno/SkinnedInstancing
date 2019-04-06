Models are all rendered in one draw call. Animation data is loaded into the GPU in two .dds textures, one for position and for rotations.
This way I can lerp these vectors using the sampler and create the matrices in the Vertex Shader. 
These textures are in the following path: ThievesLabyrinth\Assets\Animations\Enemies\Mage