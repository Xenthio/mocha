﻿namespace Mocha.Common;

public static class Notify
{
	public struct Notification
	{
		public TimeSince Lifetime { get; set; }
		public string Title { get; set; }
		public string Text { get; set; }

		public Notification( string title, string text )
		{
			Title = title;
			Text = text;
			Lifetime = 0;
		}
	}

	public static List<Notification> Notifications { get; set; } = new();

	public static void AddNotification( string title, string text, string? icon = null )
	{
		Log.Info( $"{title}: {text}" );

		var mergedTitle = (icon == null) ? title : $"{icon} {title}";
		Notifications.Add( new Notification( mergedTitle, text ) );
	}
}
