/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BonusList.h"
#include "ElevatorList.h"
#include "Game.h"
#include "GameMode.h"
#include "Explosion.h"

namespace Duel6 {
    WorldRenderer::WorldRenderer(Duel6::AppService &appService, const Duel6::Game &game)
            : font(appService.getFont()), video(appService.getVideo()), game(game), renderer(video.getRenderer()),
              target(renderer.makeTarget(video.getScreen())) {}

    void WorldRenderer::setView(const PlayerView &view) const {
        setView(view.getX(), view.getY(), view.getWidth(), view.getHeight());
    }

    void WorldRenderer::setView(int x, int y, int width, int height) const {
        renderer.setViewport(x, y, width, height);
    }

    void WorldRenderer::walls(const FaceList &walls) const {
        walls.render(game.getResources().getBlockTextures(), false);
    }

    void WorldRenderer::water(const FaceList &water) const {
        renderer.enableDepthWrite(false);
        renderer.setBlendFunc(BlendFunc::SrcColor);

        water.render(game.getResources().getBlockTextures(), false);

        renderer.setBlendFunc(BlendFunc::None);
        renderer.enableDepthWrite(true);
    }

    void WorldRenderer::sprites(const FaceList &sprites) const {
        sprites.render(game.getResources().getBlockTextures(), true);
    }

    void WorldRenderer::background(Texture texture) const {
        Int32 cw = video.getScreen().getClientWidth();
        Int32 ch = video.getScreen().getClientHeight();
        renderer.quadXY(Vector(0, 0), Vector(cw, ch), Vector(0, 1), Vector(1, -1), Material::makeTexture(texture));
    }

    void WorldRenderer::playerRankings() const {
        Float32 fontSize = 16;
        Float32 fontWidth = fontSize / 2;
        Ranking ranking = game.getMode().getRanking(game.getPlayers());
        Int32 maxNameLength = ranking.getMaxLength() + 6;

        const PlayerView &view = game.getPlayers().front().getView();
        Int32 posX = view.getX() + view.getWidth() - fontWidth * maxNameLength - 3;
        Int32 posY = view.getY() + view.getHeight() - 20;

        for (const auto &entry : ranking.entries) {
            posY = renderRankingEntry(entry, posX, posY, maxNameLength, fontSize, false);
            for (const auto &nestedRankingEntry : entry.entries) {
                posY = renderRankingEntry(nestedRankingEntry, posX, posY, maxNameLength, fontSize, false);
            }
        }
    }

    Int32 WorldRenderer::renderRankingEntry(const Ranking::Entry &entry, Int32 posX, Int32 posY, Int32 maxLength,
                                            Float32 charHeight, bool extended) const {
        if (extended) {
            maxLength += 20;
        }
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        Color fontColor(entry.fontColor);
        renderer.quadXY(Vector(posX, posY + 1), Vector((charHeight / 2) * maxLength, charHeight), entry.bcgColor);

        Int32 paddingLeft = entry.isSuperEntry() ? 0 : 5;
        renderer.setBlendFunc(BlendFunc::None);
        font.print(posX + paddingLeft, posY, 0.0f, fontColor, entry.name, charHeight);

        if (extended) {
            renderer.setBlendFunc(BlendFunc::SrcColor);
            if (!entry.isSuperEntry()) {
                fontColor = Color::YELLOW;
            }
            auto killsToDeaths = Person::getKillsToDeathsRatio(entry.kills, entry.deaths);
            font.print(posX + (charHeight / 2) * (maxLength - 23), posY, 0.0f, fontColor,
                       (Format("|{0,3}|{1,3}|{2,3}|{3,5}|{4,4}") << entry.kills << entry.assistances << entry.deaths
                                                                 << killsToDeaths << entry.points), charHeight);
        } else {
            font.print(posX + (charHeight / 2) * (maxLength - 5), posY, 0.0f, fontColor,
                       (Format("|{0,4}") << entry.points), charHeight);
        }
        return posY - charHeight;
    }

    void WorldRenderer::roundOverSummary() const {
        Float32 fontSize = 32;
        Float32 fontWidth = fontSize / 2;
        Ranking ranking = game.getMode().getRanking(game.getPlayers());
        Int32 maxLength = ranking.getMaxLength() + 6;
        Int32 maxNameLength = maxLength + 20;
        int height = fontSize * 3; // reserve for 'SCORE'
        int tableWidth = maxNameLength * (fontWidth);
        int width = std::max(tableWidth, 200);
        for (const auto &entry : ranking.entries) {
            height += fontSize * (1 + entry.entries.size());
        }
        const auto score = "---SCORE---";
        const auto kad = " K    A   D  K/D  PTS  ";
        const auto kadWidth = font.getTextWidth(kad, fontSize);
        const auto scoreWidth = font.getTextWidth(score, fontSize);

        int x = video.getScreen().getClientWidth() / 2 - width / 2;
        int y = video.getScreen().getClientHeight() / 2 - height / 2;

        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(x - fontWidth, y - fontSize), Vector(width + 2 * fontWidth, height + 2 * fontSize),
                        Color(255, 255, 255, 80));
        renderer.quadXY(Vector(x - fontWidth + 2, y - fontSize + 2),
                        Vector(width + 2 * fontWidth - 4, height + 2 * fontSize - 4), Color(0, 0, 255, 80));

        renderer.quadXY(Vector(x - fontWidth - 5, height + y - fontSize),
                        Vector(width + 2 * fontWidth + 10, fontSize + 4), Color(0, 0, 255, 255));
        renderer.setBlendFunc(BlendFunc::SrcColor);

        Int32 posX = video.getScreen().getClientWidth() / 2 - tableWidth / 2;;
        Int32 posY = y + height - fontSize * 3;

        Color fontColor = Color::WHITE;

        font.print(x + (width - scoreWidth) / 2, y + height - fontSize, 0.0f, fontColor, score, fontSize);
        font.print(x + width - kadWidth, y + height - 2 * fontSize, 0.0f, fontColor, "  K   A   D   K/D  PTS",
                   fontSize);
        for (const auto &entry : ranking.entries) {
            posY = renderRankingEntry(entry, posX, posY, maxLength, fontSize, true);
            for (const auto &nestedRankingEntry : entry.entries) {
                posY = renderRankingEntry(nestedRankingEntry, posX, posY, maxLength, fontSize, true);
            }
        }
    }

    void WorldRenderer::gameOverSummary() const {
        roundOverSummary();
    }

    void WorldRenderer::roundsPlayed() const {
        int width = 134;
        int x = video.getScreen().getClientWidth() / 2 - width / 2;
        int y = video.getScreen().getClientHeight() - 20;

        renderer.quadXY(Vector(x - 1, y - 1), Vector(width + 2, 18), Color::BLACK);
        font.print(x + 8, y, Color::WHITE,
                   Format("Rounds: {0,3}|{1,3}") << game.getCurrentRound() + 1 << game.getSettings().getMaxRounds());
    }

    void WorldRenderer::fpsCounter() const {
        std::string fpsCount = Format("FPS - {0}") << Int32(video.getFps());
        Int32 width = 8 * Int32(fpsCount.size()) + 2;

        Int32 x = Int32(video.getScreen().getClientWidth()) - width;
        Int32 y = Int32(video.getScreen().getClientHeight()) - 20;

        renderer.quadXY(Vector(x - 1, y - 1), Vector(width + 2, 18), Color::BLACK);
        font.print(x, y, Color::WHITE, fpsCount);
    }

    void WorldRenderer::youAreHere() const {
        Float32 remainingTime = game.getRound().getRemainingYouAreHere();
        if (remainingTime <= 0) return;

        renderer.enableDepthTest(false);

        Float32 radius = 0.5f + 0.5f * std::abs(D6_YOU_ARE_HERE_DURATION / 2 - remainingTime);
        for (const Player &player : game.getPlayers()) {
            Vector playerCentre = player.getCentre();
            playerCentre.z = 0.5;
            Vector lastPoint;
            for (Int32 u = 0; u < 37; u++) {
                Float32 spike = (u % 2 == 0) ? 0.95f : 1.05f;
                Vector pos = playerCentre + spike * radius * Vector::direction(u * 10);
                if (u > 0) {
                    renderer.line(lastPoint, pos, 3.0f, Color::YELLOW);
                }
                lastPoint = pos;
            }
        }

        renderer.enableDepthTest(true);
    }

    Float32
    WorldRenderer::playerIndicator(const Player &player, const Indicator &indicator, const Color &color, Float32 value,
                                   Float32 xOfs, Float32 yOfs) const {
        Float32 width = value * 0.98f;
        Float32 X = xOfs - 0.5f;
        Float32 Y = yOfs;

        Uint8 alpha = Uint8(255 * indicator.getAlpha());

        renderer.enableDepthWrite(false);
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(X, Y - 0.1f, 0.5f), Vector(1.0f, 0.1f), Color::BLACK.withAlpha(alpha));
        renderer.quadXY(Vector(X + 0.01f, Y - 0.08f, 0.5f), Vector(width, 0.07f), color.withAlpha(alpha));
        renderer.setBlendFunc(BlendFunc::None);
        renderer.enableDepthWrite(true);

        return 0.1f;
    }

    void WorldRenderer::playerName(const Player &player, const Indicator &indicator, Float32 xOfs, Float32 yOfs) const {
        const std::string &name = Format("{0}") << player.getPerson().getName();

        Float32 width = 0.15f * name.size();
        Float32 X = xOfs - width / 2;
        Float32 Y = yOfs;

        Uint8 alpha = Uint8(255 * indicator.getAlpha());

        renderer.enableDepthWrite(false);
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(X, Y, 0.5f), Vector(width, 0.3f), Color::BLUE.withAlpha(alpha));
        renderer.setBlendFunc(BlendFunc::None);

        font.print(X, Y, 0.5f, Color::YELLOW.withAlpha(alpha), name, 0.3f);

        renderer.enableDepthWrite(true);
    }

    void
    WorldRenderer::bulletIndicator(const Player &player, const Indicator &indicator, Float32 xOfs, Float32 yOfs) const {
        std::string bulletCount = Format("{0}") << player.getAmmo();

        Float32 width = 0.15f * bulletCount.size();
        Float32 X = xOfs - width / 2;
        Float32 Y = yOfs;

        Uint8 alpha = Uint8(255 * indicator.getAlpha());

        renderer.enableDepthWrite(false);

        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(X, Y, 0.5f), Vector(width, 0.3f), Color::YELLOW.withAlpha(alpha));
        renderer.setBlendFunc(BlendFunc::None);

        font.print(X, Y, 0.5f, Color::BLUE.withAlpha(alpha), bulletCount, 0.3f);

        renderer.enableDepthWrite(true);
    }

    void
    WorldRenderer::bonusIndicator(const Player &player, const Indicator &indicator, Float32 xOfs, Float32 yOfs) const {
        Uint8 alpha = Uint8(255 * indicator.getAlpha());
        auto bonusType = player.getBonus();
        Material material = Material::makeColoredTexture(game.getResources().getBonusTextures(),
                                                         Color::WHITE.withAlpha(alpha));

        Float32 size = 0.3f;
        Float32 X = xOfs - size / 2;
        Float32 Y = yOfs;

        renderer.enableDepthWrite(false);
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(X, Y, 0.5f), Vector(size, size),
                        Vector(0.3f, 0.7f, Float32(bonusType->getTextureIndex())), Vector(0.4f, -0.4f), material);
        renderer.setBlendFunc(BlendFunc::None);
        renderer.enableDepthWrite(true);
    }

    void WorldRenderer::playerStatus(const Player &player) const {
        const auto &indicators = player.getIndicators();

        if (player.isAlive() && player.isInGame()) {
            Rectangle rect = player.getCollisionRect();
            Float32 xOfs = rect.getCentre().x;
            Float32 yOfs = rect.right.y + 0.15f;

            const auto &reload = indicators.getReload();
            if (reload.isVisible()) {
                Float32 interval = player.getReloadInterval();
                Float32 value = 1.0f - std::min(interval, player.getReloadTime()) / interval;
                yOfs += playerIndicator(player, reload, Color::GREEN, value, xOfs, yOfs);
            }

            const auto &air = indicators.getAir();
            if (air.isVisible()) {
                yOfs += playerIndicator(player, air, Color::BLUE, player.getAir() / D6_MAX_AIR, xOfs, yOfs);
            }

            const auto &bonus = indicators.getBonus();
            if (bonus.isVisible()) {
                yOfs += playerIndicator(player, bonus, Color::MAGENTA,
                                        player.getBonusRemainingTime() / player.getBonusDuration(), xOfs, yOfs);
            }

            const auto &health = indicators.getHealth();
            if (health.isVisible()) {
                yOfs += playerIndicator(player, health, Color::RED, player.getLife() / D6_MAX_LIFE, xOfs, yOfs);
            }

            const auto &name = indicators.getName();
            const auto &bullets = indicators.getBullets();

            Float32 space = 0.08f;
            Float32 nameWidth = name.isVisible() ? space + player.getPerson().getName().size() * 0.15f : 0;
            std::string bulletString = Format("{0}") << player.getAmmo();
            Float32 bulletWidth = bullets.isVisible() ? space + bulletString.size() * 0.15f : 0;
            Float32 bonusWidth = bonus.isVisible() ? space + 0.3f : 0;
            Float32 totalWidth = nameWidth + bulletWidth + bonusWidth;
            Float32 halfWidth = totalWidth / 2;
            Float32 xStart = xOfs - halfWidth;

            if (name.isVisible()) {
                Float32 nameX = xStart + nameWidth / 2;
                playerName(player, name, nameX, yOfs);
            }

            if (bullets.isVisible()) {
                Float32 bulletX = xStart + nameWidth + bulletWidth / 2;
                bulletIndicator(player, bullets, bulletX, yOfs);
            }

            if (bonus.isVisible() && player.getBonus() != BonusType::NONE) {
                Float32 bonusX = xStart + nameWidth + bulletWidth + bonusWidth / 2;
                bonusIndicator(player, bonus, bonusX, yOfs);
            }

            if (name.isVisible() || bullets.isVisible() || bonus.isVisible()) {
                yOfs += 0.4f;
            }

            roundKills(player, xOfs, yOfs);
        }
    }

    void WorldRenderer::roundKills(const Player &player, Float32 xOfs, Float32 yOfs) const {
        Float32 width = (2 * player.getRoundKills() - 1) * 0.1f;
        Float32 X = xOfs + 0.05f - width / 2;
        Float32 Y = yOfs + 0.1f;

        for (Int32 i = 0; i < player.getRoundKills(); i++, X += 0.2f) {
            renderer.point(Vector(X, Y, 0.5f), 5.0f, Color::BLUE);
        }
    }

    void WorldRenderer::invulRing(const Player &player) const {
        Vector playerCentre = player.getCentre();
        Float32 radius = player.getDimensions().length() / 2.0f;
        Int32 p = Int32(player.getBonusRemainingTime() * 30) % 360;

        for (Int32 uh = p; uh < 360 + p; uh += 15) {
            Int32 u = uh % 360;
            Vector pos = playerCentre + radius * Vector::direction(u);
            renderer.point(Vector(pos.x, pos.y, 0.5f), 2.0f, Color::RED);
        }
    }

    void WorldRenderer::invulRings(const std::vector<Player> &players) const {
        for (const Player &player : players) {
            if (player.isInvulnerable()) {
                invulRing(player);
            }
        }
    }

    void WorldRenderer::splitBox(const PlayerView &view) const {
        const auto &screen = video.getScreen();
        setView(view.getX() - 2, view.getY() - 2, view.getWidth() + 4, view.getHeight() + 4);
        renderer.quadXY(Vector::ZERO, Vector(screen.getClientWidth(), screen.getClientHeight()), Color::RED);
    }

    void WorldRenderer::screenCurtain(const Color &color) const {
        const auto &screen = video.getScreen();

        video.setMode(Video::Mode::Orthogonal);
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector::ZERO, Vector(screen.getClientWidth(), screen.getClientHeight()), color);
        renderer.setBlendFunc(BlendFunc::None);
        video.setMode(Video::Mode::Perspective);
    }

    void WorldRenderer::infoMessages() const {
        const InfoMessageQueue &messageQueue = game.getRound().getWorld().getMessageQueue();

        if (game.getSettings().getScreenMode() == ScreenMode::FullScreen) {
            messageQueue.renderAllMessages(renderer, game.getPlayers().front().getView(), 20, font);
        } else {
            for (const Player &player : game.getPlayers()) {
                messageQueue.renderPlayerMessages(renderer, player, font);
            }
        }
    }

    void WorldRenderer::shotCollisionBox(const ShotList &shotList) const {
        shotList.forEach([this](const Shot &shot) -> bool {
            const auto &rect = shot.getCollisionRect();
            renderer.frame(Vector(rect.left.x, rect.left.y, 0.6f), rect.getSize(), 1.0f, Color::RED);
            return true;
        });
    }

    void WorldRenderer::setPlayerCamera(const Player &player) const {
        const Camera &camera = player.getCamera();
        Matrix viewMatrix = Matrix::lookAt(camera.getPosition(), camera.getFront(), camera.getUp());
        renderer.setViewMatrix(viewMatrix);
    }

    void WorldRenderer::renderStaticGeometry() const {
        if (game.getSettings().isWireframe()) {
            renderer.enableWireframe(true);
        }

        const World &world = game.getRound().getWorld();
        walls(world.getLevelRenderData().getWalls());
        sprites(world.getLevelRenderData().getSprites());

        renderer.enableWireframe(false);
    }

    void WorldRenderer::renderBackground() const {
        const Player &player = game.getPlayers().front(); //TODO 0-players game not working
        setView(player.getView());
        background(game.getResources().getBcgTextures().at(game.getRound().getWorld().getBackground()));
        video.setMode(Video::Mode::Perspective);

        setPlayerCamera(player);
        renderStaticGeometry();

        video.setMode(Video::Mode::Orthogonal);
    }

    void WorldRenderer::view(const Player &player) const {
        const World &world = game.getRound().getWorld();
        world.getElevatorList().render(renderer);
        world.getBonusList().render(renderer);
        world.getSpriteList().render(renderer);
        invulRings(game.getPlayers());
        water(world.getLevelRenderData().getWater());
        youAreHere();

        for (const Player &hpPlayer : game.getPlayers()) {
            playerStatus(hpPlayer);
        }
        //shotCollisionBox(world.getShotList());

        world.getExplosionList().render(renderer);
    }

    Color WorldRenderer::getGameOverOverlay() const {
        Float32 overlay = game.getRound().getRemainingGameOverWait() / D6_GAME_OVER_WAIT;
        return Color(128, 0, 0, Uint8(200 - 200 * overlay));
    }

    Color WorldRenderer::getRoundStartFadeColor(Float32 remainingTime) const {
        if (remainingTime <= 0) {
            return Color::WHITE;
        }
        Float32 intensity = 4 * (remainingTime / D6_YOU_ARE_HERE_DURATION);

        Uint8 c = 255.0f * std::max(1.0f - intensity, 0.0f);
        Color result = Color(c, c, 100 + 155.0f * std::max(1.0f - intensity, 0.0f)); //Blue darkness
        return result;
    }

    void WorldRenderer::fullScreen() const {
        const Player &player = game.getPlayers().front();
        Float32 remainingTime = game.getRound().getRemainingYouAreHere();
        setView(player.getView());

        renderer.clearBuffers(); // attempt to resolve rendering issues in Alcatraz
        Color fadeColor = remainingTime > 0 ? getRoundStartFadeColor(remainingTime) : Color::WHITE;
        target->apply(fadeColor);

        renderer.enableDepthTest(true);
        video.setMode(Video::Mode::Perspective);
        setPlayerCamera(player);
        view(player);

        if (game.getRound().hasWinner()) {
            Color overlayColor = getGameOverOverlay();
            screenCurtain(overlayColor);
        }
    }

    void WorldRenderer::splitScreen() const {
        renderer.clearBuffers();

        for (const Player &player : game.getPlayers()) {
            video.setMode(Video::Mode::Orthogonal);
            splitBox(player.getView());

            setView(player.getView());
            background(game.getResources().getBcgTextures().at(game.getRound().getWorld().getBackground()));

            video.setMode(Video::Mode::Perspective);
            setPlayerCamera(player);
            renderStaticGeometry();
            view(player);

            if (!player.isAlive()) {
                screenCurtain(Color(255, 0, 0, 128));
            }
        }
    }

    void WorldRenderer::prerender() const {
        target->record([this]() {
            renderBackground();
        });
    }

    void WorldRenderer::render() const {
        const GameSettings &settings = game.getSettings();

        if (settings.getScreenMode() == ScreenMode::FullScreen) {
            fullScreen();
        } else {
            splitScreen();
        }

        video.setMode(Video::Mode::Orthogonal);
        setView(0, 0, video.getScreen().getClientWidth(), video.getScreen().getClientHeight());

        infoMessages();

        if (settings.isShowFps()) {
            fpsCounter();
        }

        if (settings.isShowRanking() && settings.getScreenMode() == ScreenMode::FullScreen) {
            playerRankings();
        }

        if (settings.isRoundLimit()) {
            roundsPlayed();
        }

        if (game.isDisplayingScoreTab()) {
            roundOverSummary();
        }

        if (game.getRound().hasWinner()) {
            if (game.isOver()) {
                gameOverSummary();
            } else {
                roundOverSummary();
            }
        }
    }
}
