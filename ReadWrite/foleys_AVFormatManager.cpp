/*
 ==============================================================================

 Copyright (c) 2019, Foleys Finest Audio - Daniel Walz
 All rights reserved.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.

 ==============================================================================
 */

namespace foleys
{

AVFormatManager::AVFormatManager()
{
    audioFormatManager.registerBasicFormats();
}

std::shared_ptr<AVClip> AVFormatManager::createClipFromFile (VideoEngine& engine, juce::File file)
{
    auto image = juce::ImageFileFormat::loadFrom (file);
    if (image.isValid())
    {
        auto clip = std::make_shared<ImageClip> (engine);
        clip->setImage (image);
        clip->setMediaFile (file);
        return clip;
    }

    if (file.hasFileExtension ("wav;aif;aiff;mp3;wma;m4a"))
    {
        if (auto* audio = audioFormatManager.createReaderFor (file))
        {
            auto clip = std::make_shared<AudioClip> (engine);
            clip->setAudioFormatReader (audio);
            clip->setMediaFile (file);
            return clip;
        }
    }

    auto reader = AVFormatManager::createReaderFor (file);
    if (reader->isOpenedOk())
    {
        auto clip = std::make_shared<MovieClip> (engine);
        if (reader->hasVideo())
            clip->setThumbnailReader (AVFormatManager::createReaderFor (file, StreamTypes::video()));

        clip->setReader (std::move (reader));
        return clip;
    }

    return {};
}


std::unique_ptr<AVReader> AVFormatManager::createReaderFor (juce::File file, StreamTypes type)
{

#if FOLEYS_USE_FFMPEG
    return std::make_unique<FFmpegReader> (file, type);
#endif

    return {};
}

std::unique_ptr<AVWriter> AVFormatManager::createClipWriter (juce::File file)
{

#if FOLEYS_USE_FFMPEG
    auto writer = std::make_unique<FFmpegWriter>(file, "");
    return writer;
#endif
    return {};
}


} // foleys