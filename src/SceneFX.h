#ifndef SCENEFX_H
#define SCENEFX_H

#include <genesis.h>
#include "SceneUtil.h"

void FX_RunEffect();
void FX_UpdateScroll();
void FX_SetEffects(PageLayer Layer, LayerEffect Effect);
void FX_ResetEffect();
void FX_SemiResetEffect(PageLayer Layer, LayerEffect NewEffects);

#endif // SCENEFX_H 
