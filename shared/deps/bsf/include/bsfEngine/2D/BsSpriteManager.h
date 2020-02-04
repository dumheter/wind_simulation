//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "Utility/BsModule.h"
#include "2D/BsSpriteMaterial.h"

namespace bs
{
	/** @addtogroup 2D-Internal
	 *  @{
	 */

	/** Contains materials used for sprite rendering. */
	class BS_EXPORT SpriteManager : public Module<SpriteManager>
	{
		/** Types of sprite materials accessible by default. */
		enum class BuiltinSpriteMaterialType
		{
			ImageOpaque,
			ImageTransparentAlpha,
			ImageTransparentPremultiplied,
			ImageOpaqueAnimated,
			ImageTransparentAlphaAnimated,
			ImageTransparentPremultipliedAnimated,
			Text,
			Line,
			Count // Keep at end
		};

	public:
		SpriteManager();
		~SpriteManager();

		/** Returns the material used for rendering image sprites.
		 *
		 * @param[in]	transparency	Transparency mode the material should support.
		 * @param[in]	animation		True if the material should be able to perform sprite sheet animation.
		 * @return						Requested sprite material.
		 */
		SpriteMaterial* getImageMaterial(SpriteMaterialTransparency transparency, bool animation = false) const
		{
			if(!animation)
			{
				switch(transparency)
				{
				default:
				case SpriteMaterialTransparency::Opaque:
					return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageOpaque]);
				case SpriteMaterialTransparency::Alpha: 
					return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageTransparentAlpha]);
				case SpriteMaterialTransparency::Premultiplied:
					return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageTransparentPremultiplied]);
				}
			}
			else
			{
				switch(transparency)
				{
				default:
				case SpriteMaterialTransparency::Opaque:
					return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageOpaqueAnimated]);
				case SpriteMaterialTransparency::Alpha: 
					return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageTransparentAlphaAnimated]);
				case SpriteMaterialTransparency::Premultiplied:
					return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageTransparentPremultipliedAnimated]);
				}
			}
		}

		/** Returns the material used for rendering text sprites. */
		SpriteMaterial* getTextMaterial() const
			{ return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::Text]); }

		/** Returns the material used for rendering antialiased lines. */
		SpriteMaterial* getLineMaterial() const
			{ return getMaterial(builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::Line]); }

		/** Returns a sprite material with the specified ID. Returns null if one cannot be found. */
		SpriteMaterial* getMaterial(UINT32 id) const;

		/**
		 * Registers a new material in the sprite manager. Caller must ensure the material has a unique ID that doesn't
		 * already exist in the sprite manager, otherwise the call will be ignored.
		 *
		 * @return	Newly created material, or at existing one if one already exists.
		 */
		template <class T, class... Args>
		SpriteMaterial* registerMaterial(Args &&...args)
		{
			SpriteMaterial* newMaterial = bs_new<T>(std::forward<Args>(args)...);
			
			UINT32 id = newMaterial->getId();
			auto iterFind = mMaterials.find(id);
			if (iterFind != mMaterials.end())
			{
				// Already exists
				BS_LOG(Warning, Generic, "Attempting to register a sprite material that already exists, ignoring request.");
				bs_delete(newMaterial);
				return iterFind->second;
			}

			mMaterials[id] = newMaterial;
			return newMaterial;
		}
	private:
		UnorderedMap<UINT32, SpriteMaterial*> mMaterials;
		UINT32 builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::Count]{};
	};

	/** @} */
}
