﻿namespace Mocha.Renderer;

public class SkySceneObject : ModelSceneObject
{
	public float SunIntensity { get; set; } = 32.0f;
	public float PlanetRadius { get; set; } = 6372000;
	public float AtmosphereRadius { get; set; } = 6380000;

	public SkySceneObject()
	{
	}

	public override void Render( Matrix4x4 viewProjMatrix, RenderPass renderPass, CommandList commandList )
	{
		var currentCamera = SceneWorld.Current.Camera;

		var uniformBuffer = new SkyUniformBuffer
		{
			g_mModel = ModelMatrix,
			g_mViewProj = viewProjMatrix,
			g_vLightPos = SceneWorld.Current.Sun.Transform.Position,
			g_flTime = Time.Now,
			g_vLightColor = SceneWorld.Current.Sun.Color.ToVector4(),
			g_vCameraPos = SceneWorld.Current.Camera.Transform.Position,

			g_flPlanetRadius = PlanetRadius,
			g_flAtmosphereRadius = AtmosphereRadius,
			g_flSunIntensity = SunIntensity,
			g_vSunPos = SceneWorld.Current.Sun.Transform.Rotation.Backward
		};

		models.ForEach( x => x.Draw( renderPass, uniformBuffer, commandList ) );
	}
}
