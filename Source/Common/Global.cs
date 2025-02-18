﻿global using static Mocha.Common.Global;
global using Rotation = Mocha.Common.Rotation;
global using Vector2 = Mocha.Common.Vector2;
global using Vector3 = Mocha.Common.Vector3;
global using Matrix4x4 = System.Numerics.Matrix4x4;
global using Vector4 = System.Numerics.Vector4;

namespace Mocha.Common;

public static class Global
{
	public static Logger Log { get; } = new();
	public static bool IsClient { get; }
	public static UnmanagedArgs UnmanagedArgs { get; set; }
}
