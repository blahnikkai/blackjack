//
// Created by Kai Blahnik on 7/6/22.
//

#ifndef POKER_GAME_H
#define POKER_GAME_H

#include <iostream>
#include <vector>
#include "Player.h"
#include "Deck.h"
#include "Card.h"
#include "Input.h"

using std::cout, std::vector, std::string;

class Game {

public:

    explicit Game(const int & totalPlayers) :
    dealer("Dealer"), human("Human"),
    draw("Draw"), discard("Discard") {
        if(totalPlayers > 7)
            throw std::invalid_argument("Exceeded maximum of 7 total players");
        draw.fill();
        draw.shuffle();

        dealer.setHole(true);

        playerPtrs = vector<Player *>();
        bettorPtrs = vector<Bettor *>();

        bettorPtrs.push_back(&human);
        for (int i = 0; i < totalPlayers - 2; ++i) {
            bettorPtrs.push_back(new Bettor("Extra " + std::to_string(i + 1)));
        }
        std::copy(bettorPtrs.begin(), bettorPtrs.end(), std::back_inserter(playerPtrs));
        playerPtrs.push_back(&dealer);
    }

    void playGame() {
        while(true)
            playRound();
    }

    void playRound() {
        cout << "\n-------- NEW ROUND --------\n\n";
        allBet();
        enterContinue();
        allDeal();
        allTurn();
        enterContinue();
        finalCompare();
        eraseBroke();
        allDiscard();
        enterContinue();
        checkBettorsEmpty();
    }

    void checkBettorsEmpty() {
        if(bettorPtrs.empty()) {
            cout << "\nAll bettors are out of money\n"
                    "\n-------- GAME OVER --------\n\n";
            exit(0);
        }
    }

    void allBet() {
        for(Bettor * bettorPtr: bettorPtrs)
            cout << bettorPtr->getName() << " has $" << bettorPtr->getMoney() << "\n";
        for(Bettor * bettorPtr: bettorPtrs) {
            bettorPtr->betDecision();
            cout << bettorPtr->getName() << " bets $" << bettorPtr->getBet() << "\n";
        }
    }

    void allDeal() {
        dealer.setHole(true);
        for(Player * playerPtr: playerPtrs) {
            playerPtr->addCard(draw.dealCard(discard));
            playerPtr->addCard(draw.dealCard(discard));
            cout << *playerPtr << "\n";
        }
    }

    void allTurn() {
        dealer.setHole(false);
        for(Player * playerPtr: playerPtrs) {
            enterContinue();
            cout << "\n---- " << playerPtr->getName() << "'s turn ----\n";
            turn(*playerPtr);
        }
    }

    void finalCompare() {
        for(Player * playerPtr: playerPtrs) {
            cout << *playerPtr << "\n";
        }
        for(Bettor * bettorPtr: bettorPtrs) {
            int comparison = dealer.compare(*bettorPtr);
            switch(comparison) {
                case 1:
                    cout << bettorPtr->getName() << " loses $" << bettorPtr->getBet() << "\n";
                    bettorPtr->setMoney(bettorPtr->getMoney() - bettorPtr->getBet());
                    break;
                case -1:
                    cout << bettorPtr->getName() << " wins $" << bettorPtr->getBet() << "\n";
                    bettorPtr->setMoney(bettorPtr->getMoney() + bettorPtr->getBet());
                    break;
                default:
                    cout << bettorPtr->getName() << " tied\n";
                    break;
            }
        }
    }

//  if a player has no money, remove them from vectors
    void eraseBroke() {
        if(std::erase_if(bettorPtrs, [] (Bettor *b) {
            return b->getMoney() == 0;
        }) != 0) {
            playerPtrs.clear();
            std::copy(bettorPtrs.begin(), bettorPtrs.end(), std::back_inserter(playerPtrs));
            playerPtrs.push_back(&dealer);
        }
    }

    void allDiscard() {
        for(Player * playerPtr: playerPtrs)
            discard.addCards(playerPtr->discardHand());
    }

    void turn(Player & player) {
        cout << player << "\n";
        if(player.getHandValue() > 21) {
            cout << player.getName() << " busted!\n";
            return;
        }
        int decision = player.turnDecision();
        if(decision == 0) {
            cout << player.getName() << " stands\n";
            return;
        }
        cout << player.getName() << " hits\n";
        player.addCard(draw.dealCard(discard));
        turn(player);
    }

private:

    vector<Player *> playerPtrs;
    vector<Bettor *> bettorPtrs;
    Human human;
    Player dealer;
    Deck draw;
    Deck discard;

};

#endif //POKER_GAME_H
