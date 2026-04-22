#pragma once

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// ‰Šúİ’è
	virtual void Init();

	// ‰ğ•ú
	virtual void Release();

	// XVˆ—
	virtual void Update();

	// •`‰æˆ—
	virtual void Draw();

	// GUIˆ—
	virtual void ImGuiUpdate();	
};