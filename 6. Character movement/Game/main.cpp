#include "Common.h"

#include "Cube/Cube.h"
#include "Cube/RotatingCube.h"
#include "Light/RotatingPointLight.h"
#include "Game/Game.h"
#include "C:\Users\SeongYoon\source\repos\2025_Graphics\Source\Library/Character\Character.h"

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{

	std::unique_ptr<Game> game = std::make_unique<Game>(L"GGP : Lighting Cube with OOD");

    // Phong Lighting Shaders
    std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>(L"Shaders/Shader.fx", "VS", "vs_5_0");
    if (FAILED(game->GetRenderer()->AddVertexShader(L"VS", vertexShader)))
        return 0;

    std::shared_ptr<PixelShader> pixelShader = std::make_shared<PixelShader>(L"Shaders/Shader.fx", "PS", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"PS", pixelShader)))
        return 0;

    
    // 캐릭터 모델
    std::shared_ptr<Character> player = std::make_shared<Character>(L"Stone.obj"); // ← 정확한 경로 확인
    player->Translate(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));

    if (FAILED(game->GetRenderer()->AddRenderable(L"Player", player)))
    {
        MessageBoxW(nullptr, L"Player 등록 실패!", L"렌더러 오류", MB_OK);
        return 0;
    }
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"Player", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"Player", L"PS")))
        return 0;

    std::shared_ptr<PixelShader> pixelSolidShader = std::make_shared<PixelShader>(L"Shaders/Shader.fx", "PSSolid", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"PSSolid", pixelSolidShader)))
        return 0;

    // Point Light 색상 설정
    XMFLOAT4 color;
    XMStoreFloat4(&color, Colors::AntiqueWhite);
    XMFLOAT4 red_color;
    XMStoreFloat4(&red_color, Colors::Red);

    // 고정형 Point Light (흰색)
    std::shared_ptr<PointLight> fixedLight = std::make_shared<PointLight>(
        XMFLOAT4(-5.77f, 5.77f, -5.77f, 1.0f),
        color
    );
    if (FAILED(game->GetRenderer()->AddPointLight(0u, fixedLight)))
        return 0;

    // 고정 조명 큐브 (흰색)
    std::shared_ptr<Cube> lightCube = std::make_shared<Cube>(color);
    lightCube->Translate(XMVectorSet(-5.77f, 5.77f, -5.77f, 0.0f));
    if (FAILED(game->GetRenderer()->AddRenderable(L"LightCube", lightCube)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"LightCube", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"LightCube", L"PSSolid")))
        return 0;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    TODO1: Rotating Point Light
    -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    std::shared_ptr<PointLight> rotatingLight = std::make_shared<RotatingPointLight>(
        XMFLOAT4(5.77f, 0.0f, -5.77f, 1.0f),
        red_color
    );
    if (FAILED(game->GetRenderer()->AddPointLight(1u, rotatingLight)))
        return 0;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    TODO2: Rotating Point Light Cube
    -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    std::shared_ptr<BaseCube> rotatingLightCube = std::make_shared<RotatingCube>(red_color);
    rotatingLightCube->Translate(XMVectorSet(5.77f, 0.0f, -5.77f, 0.0f));
    if (FAILED(game->GetRenderer()->AddRenderable(L"RotatingLightCube", rotatingLightCube)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"RotatingLightCube", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"RotatingLightCube", L"PSSolid")))
        return 0;

    // Phong Cube
    std::shared_ptr<Cube> phongCube = std::make_shared<Cube>(L"diffuso.tif");
    if (FAILED(game->GetRenderer()->AddRenderable(L"PhongCube", phongCube)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"PhongCube", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"PhongCube", L"PS")))
        return 0;


    if (FAILED(game->Initialize(hInstance, nCmdShow)))
        return 0;

    return game->Run();
}