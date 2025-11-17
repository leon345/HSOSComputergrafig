#include "Coin.h"

void Coin::update(float dtime)
{
    if (!getIsActive()) return;
    
    time += dtime;
    Matrix trans, rotY, rotX;
    if (state == Idle)
    {
        // Rotation um die Y-Achse (ständig drehend)
        float spinSpeed = 1.5f; // Umdrehungen pro Sekunde
        float angle = spinSpeed * time;

        // Auf- und Abbewegung
        float bobHeight = 0.25f;
        float bob = sinf(time * 2.0f) * bobHeight;

        // Basisposition (wo die Münze ursprünglich steht)
        //Vector basePos = initialPosition; // solltest du im Konstruktor speichern

        // Transformation zusammenbauen:
        trans.translation(basePos.X, basePos.Y + bob, basePos.Z);

        // kippt die Münze einmalig aufrecht
        rotX.rotationX(M_PI / 2.0f);

        // dreht die Münze um die Y-Achse
        rotY.rotationY(angle);
    } else if (state == Collected)
    {
        collectTime += dtime;
        
        float spinSpeed = 10.0f;
        float angle = spinSpeed * time;
        float bob = collectTime * 3.0f;

        trans.translation(basePos.X, basePos.Y + bob, basePos.Z);
        rotX.rotationX(M_PI / 2.0f);
        rotY.rotationY(angle);
        if (collectTime > 1.0f)
        {
            setIsActive(false);
        }
    }
    

    // Reihenfolge wichtig: erst X, dann Y, dann Translation
    Matrix final = trans * rotY * rotX;

    transform(final);
}

void Coin::collect()
{
    if (state == Idle)
    {
        setIsCollidable(false);
        setIsCollectable(false);
        state = Collected;
        collectTime = 0.0f;
    }
}