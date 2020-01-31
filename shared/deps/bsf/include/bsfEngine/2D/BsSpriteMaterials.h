//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "2D/BsSpriteMaterial.h"

namespace bs
{
	/** @addtogroup 2D-Internal
	 *  @{
	 */

	/** Sprite material used for rendering transparent images. */
	class BS_EXPORT SpriteImageTransparentMaterial : public SpriteMaterial
	{
	public:
		SpriteImageTransparentMaterial();
	};

	/** Sprite material used for rendering opaque images. */
	class BS_EXPORT SpriteImageOpaqueMaterial : public SpriteMaterial
	{
	public:
		SpriteImageOpaqueMaterial();
	};

	/** Sprite material used for rendering text. */
	class BS_EXPORT SpriteTextMaterial : public SpriteMaterial
	{
	public:
		SpriteTextMaterial();
	};

	/** Sprite material used for antialiased lines. */
	class BS_EXPORT SpriteLineMaterial : public SpriteMaterial
	{
	public:
		SpriteLineMaterial();
	};

	/** @} */
}