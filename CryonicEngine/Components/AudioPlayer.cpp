#include "AudioPlayer.h"
#ifndef EDITOR
#include "../Game.h"
#endif

void AudioPlayer::Start()
{
	if (playOnStart && audioClip != nullptr)
		Play();
}

void AudioPlayer::Update()
{
	if (audioClip != nullptr && audioLoaded && !audioClip->ShouldLoadInMemory())
		Raylib::UpdateMusicStream(music);

	for (auto it = musicStreams.begin(); it != musicStreams.end();)
	{
		if (Raylib::IsMusicReady(*it) && !Raylib::IsMusicStreamPlaying(*it)) // Removes the music from the vector if its finished playing
			it = musicStreams.erase(it);
		else
		{
			Raylib::UpdateMusicStream(*it);
			++it;
		}
	}
}

void AudioPlayer::Destroy()
{
	Stop();

	for (Raylib::Music& musicStream : musicStreams)
		Raylib::StopMusicStream(musicStream);

	if (this->audioClip != nullptr)
		delete this->audioClip;
}

void AudioPlayer::SetAudioClip(AudioClip audioClip)
{
	if (this->audioClip != nullptr)
	{
		Stop();
		delete this->audioClip;
	}

	if (audioClip.GetPath().empty() || audioClip.GetPath() == "nullptr") // It may be "nullptr" if no audio clip was set in the properties
	{
		ConsoleLogger::WarningLog(gameObject->GetName() + ":Audio Player - SetAudioClip(AudioClip audioClip) failed to set audio clip. The selected audio clip has an invalid audio file path.", false);
		return;
	}

	this->audioClip = new AudioClip(audioClip.GetPath());
}

bool AudioPlayer::IsPlaying() const
{
	if (audioClip == nullptr || !audioLoaded)
		return false;

	if (audioClip->ShouldLoadInMemory())
		return Raylib::IsSoundPlaying(sound);
	else
		return Raylib::IsMusicStreamPlaying(music);
}

void AudioPlayer::Play()
{
	if (audioClip == nullptr)
	{
		ConsoleLogger::WarningLog(gameObject->GetName() + ":Audio Player - Play() failed to play audio. No audio clip selected. Use SetAudioClip() before Play() or Play(AudioClip audioClip).", false);
		return;
	}
	std::filesystem::path path;
#ifndef EDITOR
	path = std::filesystem::path(exeParent) / "Resources" / "Assets" / audioClip->GetPath();
#endif
	if (audioClip->GetPath().empty() || audioClip->GetPath() == "nullptr" || !std::filesystem::exists(path)) // It may be "nullptr" if no audio clip was set in the properties
	{
		ConsoleLogger::WarningLog(gameObject->GetName() + ":Audio Player - Play() failed to play audio. The selected audio clip has an invalid audio file path.", false);
		return;
	}

	if (audioClip->ShouldLoadInMemory())
	{
		// Todo: Add this
		// When should they be unloaded, game close?
		Raylib::PlaySound(sound); // Does it need to wait until sound is loaded to call this?
	}
	else
	{
		music = Raylib::LoadMusicStream(path.string().c_str());
		music.looping = loop;
		Raylib::PlayMusicStream(music);
	}
	audioLoaded = true;
	paused = false;
}

void AudioPlayer::Play(AudioClip audioClip)
{
	std::filesystem::path path;
#ifndef EDITOR
	path = std::filesystem::path(exeParent) / "Resources" / "Assets" / audioClip.GetPath();
#endif
	if (audioClip.GetPath().empty() || audioClip.GetPath() == "nullptr" || !std::filesystem::exists(path)) // It may be "nullptr" if no audio clip was set in the properties
	{
		ConsoleLogger::WarningLog(gameObject->GetName() + ":Audio Player - Play(AudioClip audioClip) failed to play audio. The selected audio clip has an invalid audio file path.", false);
		return;
	}
	// Todo: Ensure audio clip works and send errror if not (like check if the path exists)

	if (audioClip.ShouldLoadInMemory())
	{
		// Todo: Add this
		// When should they be unloaded, game close?
		//Raylib::PlaySound(); // Does it need to wait until sound is loaded to call this?
	}
	else
	{
		musicStreams.push_back(Raylib::LoadMusicStream((path).string().c_str()));
		Raylib::PlayMusicStream(music);
	}
}

void AudioPlayer::Stop()
{
	if (!IsPlaying())
		return;

	if (audioClip->ShouldLoadInMemory())
		Raylib::StopSound(sound);
	else
		Raylib::StopMusicStream(music);
	audioLoaded = false;
}

void AudioPlayer::Pause()
{
	paused = true;
	if (audioClip == nullptr)
		return;

	if (audioClip->ShouldLoadInMemory())
		Raylib::PauseSound(sound);
	else
		Raylib::PauseMusicStream(music);
}

void AudioPlayer::Unpause()
{
	paused = false;
	if (audioClip == nullptr)
		return;

	if (audioClip->ShouldLoadInMemory())
		Raylib::ResumeSound(sound);
	else
		Raylib::ResumeMusicStream(music);
}

bool AudioPlayer::IsPaused() const
{
	return paused;
}

void AudioPlayer::SetLooping(bool loop)
{
	this->loop = loop;
}

bool AudioPlayer::IsLooping() const
{
	return loop;
}
