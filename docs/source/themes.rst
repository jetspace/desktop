Themes
=======

What Are Themes?
-----------------

Themes change the look of your desktop environment!
You can setup how your panel, the context menus, etc... will look like!

What do I need to write Themes?
--------------------------------

You will basically just need to be able to understand CSS (Cascading Style Sheets).
It will help you if you have written GTK3 themes before, because SiDE is written in GTK3
it's themes will be using the GTK3 Theming Engine.

Overview
---------

You should imagine the GtkWindows as Websites written in HTML.

In the following example I will demonstrate this on the panel:

<SiDEPanel>

--<SiDEPanelBox>

----<GtkItems/>

----<SiDEPanelAppMenuButton/>

----<SiDEPanelHiddenApp/>

----<SiDEPanelAppIcon/>

--</SiDEPanelBox>

</SiDEPanel>


All Names:
-----------
- #SiDEContextMenuDesktop
- #SiDEPanel
- #SiDEPanelBox
- #SiDEPanelAppMenuButton
- #SiDEPanelHiddenApp
- #SiDEPanelAppIcon

Default Plug-ins

- #SiDEPluginUserContextLogout
- #SiDEPanelPluginCalendarBox
- #SiDEPanelPluginCalendarWindow
- #SiDEPanelPluginCalendar

Examples:
----------

You can find examples in the Project Repository
