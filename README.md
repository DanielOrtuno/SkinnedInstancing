# SkinnedInstancing
DirectX11 Skinned Instancing

I did this small project to test a feature we wanted to add to a game. Animation data was saved to two different textures (one describes joint positions, the other one joint rotation.) The idea was that instead of sending the joint data to the vertex shader through constant buffers, I could use textures and sample them in the shader. It worked great for what I wanted to do.
