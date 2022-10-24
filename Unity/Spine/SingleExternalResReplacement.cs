// Copyright 2022 Gocheng Ran <rangaocheng@outlook.com>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

using Spine;
using Spine.Unity;
using Spine.Unity.AttachmentTools;
using UnityEngine;

public class SingleExternalResReplacement : MonoBehaviour
{
    [Header("Control")]

    public SkeletonAnimation hero;

    public string texPath;
    public string slotName;

    // Start is called before the first frame update
    void Start()
    {
        changePartialSkin();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void ChangePartialSkin()
    {
        if (string.IsNullOrEmpty(slotName) || string.IsNullOrEmpty(texPath))
            return;

        SkeletonRenderer heroRenderer = (SkeletonRenderer)hero;
        Skeleton skeleton = heroRenderer.Skeleton;

        Slot slot = skeleton.FindSlot(slotName);
        Texture2D tex = Resources.Load<Texture2D>(texPath);

        if (slot == null || tex == null)
            return;

        UpdatePartialSkin(skeleton, tex, slot);
    }


    public static void UpdatePartialSkin(Skeleton skeleton, Texture2D texture, Slot slot)
    {
        RegionAttachment attachment = (RegionAttachment)slot.Attachment;

        AtlasPage page = new AtlasPage();
        page.name = texture.name;
        page.uWrap = TextureWrap.ClampToEdge;
        page.vWrap = TextureWrap.ClampToEdge;


        AtlasRegion originRegion = attachment.RendererObject as AtlasRegion;
        Material material = (Material)originRegion.page.rendererObject;
        Material newMaterial = new Material(material);
        newMaterial.mainTexture = texture;


        page.width = texture.width;
        page.height = texture.height;
        page.rendererObject = newMaterial;

        AtlasRegion region = new AtlasRegion();
        region.page = page;
        region.width = texture.width;
        region.height = texture.height;
        region.originalWidth = texture.width;
        region.originalHeight = texture.height;
        region.rotate = false;
        region.u = 0;
        region.v = 0;
        region.u2 = 1;
        region.v2 = 1;

        attachment.SetRegion(region);  
    }
}
