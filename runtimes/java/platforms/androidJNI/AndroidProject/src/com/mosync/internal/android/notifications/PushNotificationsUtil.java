/* Copyright 2013 David Axmark

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package com.mosync.internal.android.notifications;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

import static com.mosync.internal.generated.MAAPI_consts.MA_NOTIFICATION_DISPLAY_FLAG_DEFAULT;

/**
 * Utility class for storing and accessing
 * editor preferences.
 * Create a new Editor for these preferences, through which
 * you can make modifications to the data in the preferences
 * and atomically commit those changes back to the SharedPreferences object.
 * @author emma tresanszki
 *
 */
public class PushNotificationsUtil
{
	/**
	 * The name of the extra data sent when launching MoSync activity.
	 * Used at incoming push notifications.
	 */
	public static String MOSYNC_INTENT_EXTRA_MESSAGE = "com.mosync.java.android.IntentExtra";
	public static String MOSYNC_INTENT_EXTRA_NOTIFICATION_HANDLE = "push.notification.handle";
	public static String MOSYNC_INTENT_EXTRA_NOTIFICATION = "push.notification";

	// Fields to store in Editor preferences.
    public static final String PREFERENCE = "com.mosync.internal.android.notifications";
    public static final String NOTIFICATION_TITLE = "title";
    public static final String NOTIFICATION_TICKER = "ticker";
    public static final String NOTIFICATION_DISPLAY_FLAG = "displayFlag";

    /**
     * Get the registration back off time.
     * @param context
     * @return The preferred title, or
     * an empty string if it was not defined.
     */
    static String getNotificationTitle(Context context)
    {
        final SharedPreferences prefs = context.getSharedPreferences(
                PREFERENCE,
                Context.MODE_PRIVATE);

        return prefs.getString(NOTIFICATION_TITLE,"");
    }

    /**
     * Store push notifications title,
     * so it can be accessed for incoming notifications.
     *
     * @param context
     * @param text The notification title.
     */
    static void setPushNotificationTitle(Context context, String text)
    {
        final SharedPreferences prefs = context.getSharedPreferences(
                PREFERENCE,
                Context.MODE_PRIVATE);
        Editor editor = prefs.edit();
        editor.putString(NOTIFICATION_TITLE, text);
        editor.commit();
    }

    /**
     * Get the text that was set for notification title.
     * @param context
     * @return The preferred title, or
     * an empty string if it was not defined.
     */
    static String getNotificationTicker(Context context)
    {
        final SharedPreferences prefs = context.getSharedPreferences(
                PREFERENCE,
                Context.MODE_PRIVATE);

        return prefs.getString(NOTIFICATION_TITLE,"");
    }

    /**
     * Store push notifications ticker text,
     * so it can be accessed for incoming notifications.
     *
     * @param context
     * @param text The ticker text for the status bar.
     */
    static void setPushNotificationTicker(Context context, String text)
    {
        final SharedPreferences prefs = context.getSharedPreferences(
                PREFERENCE,
                Context.MODE_PRIVATE);
        Editor editor = prefs.edit();
        editor.putString(NOTIFICATION_TITLE, text);
        editor.commit();
    }

    /**
     * Store push notifications display flag,
     * so it can be accessed for incoming notifications.
     *
     * @param context
     * @param flag The display flag.
     */
    static void setPushNotificationDisplayFlag(Context context, int flag)
    {
        final SharedPreferences prefs = context.getSharedPreferences(
                PREFERENCE,
                Context.MODE_PRIVATE);
        Editor editor = prefs.edit();
        editor.putInt(NOTIFICATION_DISPLAY_FLAG, flag);
        editor.commit();
    }

    /**
     * Get the text that was set for notification title.
     * @param context
     * @return The preferred display flag, or
     * 0 if it was not defined.
     */
    @SuppressWarnings("deprecation")
	public static int getPushNotificationDisplayFlag(Context context)
    {
        final SharedPreferences prefs = context.getSharedPreferences(
                PREFERENCE,
                Context.MODE_PRIVATE);

        return prefs.getInt(NOTIFICATION_DISPLAY_FLAG, MA_NOTIFICATION_DISPLAY_FLAG_DEFAULT);
    }
}