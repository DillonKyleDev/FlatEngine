#include "Texture.h"
#include "FlatEngine.h"
#include "WindowManager.h"


namespace FlatEngine
{
	Texture::Texture(std::string path)
	{
		m_path = path;
		m_texture = NULL;
		m_surface = NULL;
		m_textureWidth = 0;
		m_textureHeight = 0;
		if (path != "")
		{
			LoadFromFile(path);
		}
	}

	Texture::~Texture()
	{
		FreeTexture();
		FreeSurface();
	}

	bool Texture::LoadFromFile(std::string path)
	{
		m_path = path;

		if (path != "")
		{
			FreeTexture();
			SDL_Texture* newTexture = NULL;
			SDL_Surface* loadedSurface = IMG_Load(path.c_str());

			if (loadedSurface == NULL)
			{
				LogError("Unable to load image %s! SDL_image Error: %s\n" + path + IMG_GetError());
			}
			else
			{
				// Color key image **Whatever color you put in here it will treat as transparent**
				SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 0));

				// Create texture from surface pixels
				newTexture = SDL_CreateTextureFromSurface(F_Window->GetRenderer(), loadedSurface);
				if (newTexture == NULL)
				{
					LogError("Unable to create texture from %s! SDL Error: %s\n" + path + SDL_GetError());
				}
				else
				{
					m_textureWidth = loadedSurface->w;
					m_textureHeight = loadedSurface->h;
				}

				SDL_FreeSurface(loadedSurface);
			}

			m_texture = newTexture;
			return m_texture != NULL;
		}
		else return false;
	}

	bool Texture::LoadSurface(std::string path, SDL_Surface* screenSurface)
	{
		FreeSurface();

		//The final optimized image
		SDL_Surface* optimizedSurface = NULL;		
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());

		if (loadedSurface == NULL)
		{
			LogError("Unable to create texture from %s! SDL Error: %s\n" + path + SDL_GetError());
		}
		else
		{
			//Convert surface to screen format
			optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, 0);
			if (m_surface == NULL)
			{
				printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}

			SDL_FreeSurface(loadedSurface);
		}

		m_surface = optimizedSurface;
		return m_surface != NULL;
	}

	//Creates image from font string
	bool Texture::LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font)
	{
		FreeSurface();		
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
		//TTF_RenderText_Solid_Wrapped(); For wrapped text

		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			m_texture = SDL_CreateTextureFromSurface(F_Window->GetRenderer(), textSurface);

			if (m_texture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				m_textureWidth = textSurface->w;
				m_textureHeight = textSurface->h;
			}

			SDL_FreeSurface(textSurface);
		}

		return m_texture != NULL;
	}


	void Texture::FreeTexture()
	{			
		if (m_texture != NULL)
		{
			SDL_DestroyTexture(m_texture);
			m_texture = NULL;
			m_textureWidth = 0;
			m_textureHeight = 0;
		}
	}

	SDL_Texture* Texture::GetTexture()
	{
		return m_texture;
	}

	void Texture::FreeSurface()
	{
		if (m_surface != NULL)
		{
			SDL_FreeSurface(m_surface);
			m_surface = NULL;
		}
	}

	int Texture::GetWidth()
	{
		return m_textureWidth;
	}
	int Texture::GetHeight()
	{
		return m_textureHeight;
	}

	void Texture::SetDimensions(int width, int height)
	{
		m_textureWidth = width;
		m_textureHeight = height;
	}
}