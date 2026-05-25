#include "raylib.h"
#include "rlgl.h" //necesario para rotaciones matriciales
#include <cmath>
#include <vector>

//funciones Auxiliares para matematicas
//aproximación de seno usando series de taylor con 3 terminos
float TaylorSine(float x) {
    // sen(x) ≈ x - x^3/3! + x^5/5!
    return x - (powf(x, 3.0f) / 6.0f) + (powf(x, 5.0f) / 120.0f);
}

int main() {
    //configuracion inicial
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Hands-on 4: Actividad Integradora - Simulacion 3D Avanzada");

    Camera3D camera = { 0 };
    camera.position = { 0.0f, 25.0f, 25.0f }; //ajustamos la camara
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);

    //variables de Estado para las transformaciones
    //1.traslación continua
    float posTraslacionX = -4.0f;
    float velTraslacion = 4.0f; 

    //2.rotación
    float anguloRotacion = 0.0f;
    float velRotacion = 90.0f; //grados por segundo

    //3. rebote colision elastica
    Vector3 posRebote = { 0.0f, 1.0f, 0.0f };
    Vector3 velRebote = { 3.0f, 5.0f, 2.0f };
    float radioRebote = 0.4f;
    float limiteCaja = 2.0f;

    //4. movimiento senoidal oscilacion armonica
    float tiempoSenoidal = 0.0f;
    float posXSenoidal = -2.0f;
    float velXSenoidal = 1.0f;

    //5.trayectoria parabolica
    Vector3 posParabola = { -2.0f, 0.0f, 0.0f };
    Vector3 velParabola = { 1.5f, 6.0f, 0.0f };
    float gravedad = -9.81f; 

    //6. orbita
    float anguloOrbita = 0.0f;
    float velOrbita = 2.0f;
    float radioOrbita = 2.0f;

    //9.multiple linear regression datos normalizados
    //usando los coeficientes optimos calculados: B0=-153.51, B1=1.24, B2=12.08
    //se escalan para que la grafica encaje visualmente en una zona de 4x4 unidades.
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float t = (float)GetTime();
        UpdateCamera(&camera, CAMERA_ORBITAL);

        //LÓGICA DE ACTUALIZACIÓN MATEMÁTICA
        
        //1.traslación integración de velocidad
        posTraslacionX += velTraslacion * dt;
        if (posTraslacionX > 4.0f) posTraslacionX = -4.0f; //screen wrapping

        //2. rotación acumulación angular
        anguloRotacion += velRotacion * dt;

        //3. rebote detección de limites e inversion de vector normal
        posRebote.x += velRebote.x * dt;
        posRebote.y += velRebote.y * dt;
        posRebote.z += velRebote.z * dt;
        
        if (posRebote.x >= limiteCaja - radioRebote || posRebote.x <= -limiteCaja + radioRebote) velRebote.x *= -1.0f;
        if (posRebote.y >= limiteCaja * 2 - radioRebote || posRebote.y <= radioRebote) velRebote.y *= -1.0f;
        if (posRebote.z >= limiteCaja - radioRebote || posRebote.z <= -limiteCaja + radioRebote) velRebote.z *= -1.0f;

        //4. senoidal ecuacion paramétrica con vector tiempo
        tiempoSenoidal += dt;
        posXSenoidal += velXSenoidal * dt;
        if (posXSenoidal > 2.0f) { posXSenoidal = -2.0f; tiempoSenoidal = 0.0f; }
        float posYSenoidal = 2.0f + sinf(tiempoSenoidal * 3.0f) * 1.5f;

        //5. parabolica integracion numerica de euler
        velParabola.y += gravedad * dt;
        posParabola.x += velParabola.x * dt;
        posParabola.y += velParabola.y * dt;
        if (posParabola.y < 0.0f) { 
            posParabola = { -2.0f, 0.0f, 0.0f };
            velParabola = { 1.5f, 6.0f, 0.0f };
        }

        //6. orbita trigonometría del círculo unitario
        anguloOrbita += velOrbita * dt;
        float orbX = cosf(anguloOrbita) * radioOrbita;
        float orbZ = sinf(anguloOrbita) * radioOrbita;

        //RENDERIZADO 3D
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        //cuadrícula principal
        DrawGrid(40, 1.0f);

        //ZONA 1: traslacion continua arriba izquierda
        rlPushMatrix();
            rlTranslatef(-8.0f, 0.0f, -8.0f);
            DrawCube({ posTraslacionX, 0.5f, 0.0f }, 1.0f, 1.0f, 1.0f, BLUE);
            DrawCubeWires({ posTraslacionX, 0.5f, 0.0f }, 1.0f, 1.0f, 1.0f, DARKBLUE);
            DrawLine3D({-4.0f, 0.0f, 0.0f}, {4.0f, 0.0f, 0.0f}, GRAY);
        rlPopMatrix();

        //ZONA 2: rotación arriba centro
        rlPushMatrix();
            rlTranslatef(0.0f, 1.5f, -8.0f);
            rlRotatef(anguloRotacion, 1.0f, 1.0f, 0.0f); // Matriz de rotación en ejes X y Y
            DrawCube({ 0.0f, 0.0f, 0.0f }, 1.5f, 1.5f, 1.5f, RED);
            DrawCubeWires({ 0.0f, 0.0f, 0.0f }, 1.5f, 1.5f, 1.5f, MAROON);
        rlPopMatrix();

        //ZONA 3: rebote elastico Arriba Derecha
        rlPushMatrix();
            rlTranslatef(8.0f, 0.0f, -8.0f);
            DrawCubeWires({ 0.0f, limiteCaja, 0.0f }, limiteCaja*2, limiteCaja*2, limiteCaja*2, LIGHTGRAY);
            DrawSphere(posRebote, radioRebote, GREEN);
        rlPopMatrix();

        //ZONA 4: movimiento senoidal centro izquierda
        rlPushMatrix();
            rlTranslatef(-8.0f, 0.0f, 0.0f);
            DrawLine3D({-2.0f, 2.0f, 0.0f}, {2.0f, 2.0f, 0.0f}, LIGHTGRAY); 
            DrawSphere({ posXSenoidal, posYSenoidal, 0.0f }, 0.4f, PURPLE);
        rlPopMatrix();

        //ZONA 5: parabolica centro
        rlPushMatrix();
            rlTranslatef(0.0f, 0.0f, 0.0f);
            DrawSphere(posParabola, 0.3f, ORANGE);
            //dibujar rastro
            for(float i = 0; i < 2.0f; i+=0.1f) {
                float simX = -2.0f + (1.5f * i);
                float simY = (6.0f * i) + (0.5f * gravedad * i * i);
                if(simY > 0) DrawPoint3D({simX, simY, 0.0f}, RED);
            }
        rlPopMatrix();

        //ZONA 6: orbita centro derecha
        rlPushMatrix();
            rlTranslatef(8.0f, 0.0f, 0.0f);
            DrawSphere({ 0.0f, 1.0f, 0.0f }, 0.5f, YELLOW); 
            DrawCircle3D({ 0.0f, 1.0f, 0.0f }, radioOrbita, { 1.0f, 0.0f, 0.0f }, 90.0f, LIGHTGRAY);
            DrawSphere({ orbX, 1.0f, orbZ }, 0.3f, DARKBLUE);
        rlPopMatrix();

        //ZONA 7: series de taylor abajo izquierda
        rlPushMatrix();
            rlTranslatef(-8.0f, 0.0f, 8.0f);
            for (float x = -3.0f; x <= 3.0f; x += 0.1f) {
                
                DrawSphere({ x, 2.0f + sinf(x), 0.0f }, 0.05f, BLUE);
                
                DrawSphere({ x, 2.0f + TaylorSine(x), 0.5f }, 0.05f, RED);
            }
        rlPopMatrix();

        //ZONA 8: regresion lineal simple 
        rlPushMatrix();
            rlTranslatef(0.0f, 0.0f, 8.0f);
            // Dispersión simulada
            DrawSphere({-1.5f, 0.5f, 0.0f}, 0.1f, BLACK);
            DrawSphere({-0.5f, 1.2f, 0.0f}, 0.1f, BLACK);
            DrawSphere({0.5f, 2.8f, 0.0f}, 0.1f, BLACK);
            DrawSphere({1.5f, 3.5f, 0.0f}, 0.1f, BLACK);
            
            DrawLine3D({-2.0f, 0.0f, 0.0f}, {2.0f, 4.0f, 0.0f}, MAGENTA);
        rlPopMatrix();

        //ZONA 9: regresion lineal multiple 
        //representacion tridimensional del dataset de experimentos quimicos
        rlPushMatrix();
            rlTranslatef(8.0f, 0.0f, 8.0f);
            //plano de regresión transparente
            DrawPlane({0.0f, 2.0f, 0.0f}, {4.0f, 4.0f}, Fade(SKYBLUE, 0.5f));
            //puntos de datos X1 y X2 proyectados en espacio cartesiano normalizado
            DrawSphere({-1.0f, 2.5f, -1.0f}, 0.15f, DARKGREEN);
            DrawSphere({1.0f, 3.5f, 1.0f}, 0.15f, DARKGREEN);
            DrawSphere({0.0f, 1.5f, 0.5f}, 0.15f, DARKGREEN);
            DrawLine3D({-1.0f, 2.0f, -1.0f}, {-1.0f, 2.5f, -1.0f}, RED); //error residual Epsilon
        rlPopMatrix();

        EndMode3D();

        //HUD / INTERFAZ 2D 
        DrawRectangle(0, 0, screenWidth, 80, Fade(DARKGRAY, 0.8f));
        DrawText("Hands-on 4: Actividad Integradora - UDG CUCEI", 20, 20, 20, WHITE);
        DrawText(TextFormat("FPS: %i", GetFPS()), screenWidth - 100, 20, 20, GREEN);

        //etiquetas de Zonas
        DrawText("1. Traslacion", 200, 150, 20, DARKBLUE);
        DrawText("2. Rotacion Isometrica", 700, 150, 20, MAROON);
        DrawText("3. Rebote Elastico", 1200, 150, 20, DARKGREEN);
        
        DrawText("4. Mov. Senoidal", 200, 450, 20, PURPLE);
        DrawText("5. Tiro Parabolico", 700, 450, 20, ORANGE);
        DrawText("6. Orbita Uniforme", 1200, 450, 20, GOLD);

        DrawText("7. Series de Taylor", 200, 750, 20, RED);
        DrawText("8. Simple Linear Reg (SLR)", 700, 750, 20, MAGENTA);
        DrawText("9. Multiple Linear Reg (MLR)", 1200, 750, 20, LIME);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
