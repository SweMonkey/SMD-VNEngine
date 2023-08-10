#ifndef SCENEFX_H
#define SCENEFX_H

#include <genesis.h>
#include "SceneUtil.h"

void CalcEffect();
void RunEffectVSYNC();
void SetEffects(PageLayer Layer, LayerEffect Effect);
void ResetEffect();
void SemiResetEffect(PageLayer Layer, LayerEffect NewEffects);

#endif // SCENEFX_H 
