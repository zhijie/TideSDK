/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _WIN32_UI_BINDING_H_
#define _WIN32_UI_BINDING_H_

#define WEB_INSPECTOR_MENU_ITEM_ID 7500
#define NEXT_ITEM_ID_BEGIN 7501

namespace ti
{
    class Win32UIBinding : public UIBinding
    {
        public:
        Win32UIBinding(Module* uiModule, Host *host);
        ~Win32UIBinding();
        AutoMenu CreateMenu();
        AutoMenuItem CreateMenuItem();
        AutoMenuItem CreateSeparatorMenuItem();
        AutoMenuItem CreateCheckMenuItem();
        void SetMenu(AutoMenu);
        AutoTrayItem AddTray(std::string& icon_path, TiMethodRef cbSingleClick);
        void SetContextMenu(AutoMenu);
        void SetIcon(std::string& iconPath);
        long GetIdleTime();

        AutoMenu GetMenu();
        AutoMenu GetContextMenu();
        std::string& GetIcon();
        static UINT nextItemId;

        static HICON LoadImageAsIcon(std::string& path, int sizeX, int sizeY);
        static HBITMAP LoadImageAsBitmap(std::string& path, int sizeX, int sizeY);
        static HICON BitmapToIcon(HBITMAP bitmap, int sizeX, int sizeY);
        static HBITMAP IconToBitmap(HICON icon, int sizeX, int sizeY);
        static HBITMAP LoadPNGAsBitmap(std::string& path, int sizeX, int sizeY);
        static cairo_surface_t* ScaleCairoSurface(
            cairo_surface_t *oldSurface, int newWidth, int newHeight);
        static void ReleaseImage(HANDLE);
        static void SetProxyForURL(std::string& url);
        static void ErrorDialog(std::string);

        private:
        AutoPtr<Win32Menu> menu;
        AutoPtr<Win32Menu> contextMenu;
        std::string iconPath;
        static std::vector<HICON> loadedICOs;
        static std::vector<HBITMAP> loadedBMPs;
    };
}

#endif
