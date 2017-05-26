#include "gamescene.h"
#include <memory>
#define ELEMENTSIZE 60
#define DISTANCE ELEMENTSIZE*2

GameScene::GameScene(QLabel * moveInfoLabel)
{
    this->moveInfo = moveInfoLabel;
    this->whitePlayerMove = true;
    this->isPawnSelected = false;
    this->isDuringMove = false;
    this->blackBrush = QBrush(Qt::black);
    this->grayBrush = QBrush(Qt::gray);
    this->whiteKingBrush = QBrush(Qt::lightGray); // tu do zmiany
    this->blackKingBrush = QBrush(Qt::blue); // tu do zmiany
    this->outlinePen4Chessboard = QPen(Qt::red);
    this->outlinePen4Pawn = QPen(Qt::blue);
    this->outlinePen4SelectedPawn = QPen(Qt::yellow);
    this->whiteBrush = QBrush(Qt::white);
    this->outlinePen4Chessboard.setWidth(1);
    this->outlinePen4Pawn.setWidth(2);
    SetChessboard(this);
    SetPawns(this);
}

Pawn * GameScene::PawnMaker(int x, int y, QGraphicsScene * scene, QPen pen, QBrush brush)
{
    Pawn * result = new Pawn();
    result->pawn = scene->addEllipse(0, 0, ELEMENTSIZE, ELEMENTSIZE, pen, brush);
    result->pawn->setPos(x * ELEMENTSIZE, y * ELEMENTSIZE);
    return result;
}

QGraphicsRectItem * GameScene::FieldMaker(int x, int y, QGraphicsScene * scene, QPen pen, QBrush brush )
{
    QGraphicsRectItem * result = new QGraphicsRectItem();
    result = scene->addRect(0, 0, ELEMENTSIZE, ELEMENTSIZE, pen, brush);
    result->setPos(x* ELEMENTSIZE, y * ELEMENTSIZE);
    return result;
}

void GameScene::SetChessboard(QGraphicsScene* scene)
{
    bool nowWhite = true;
    for (auto y = 0 ; y < 8 ; ++y)
    {
        for(auto x = 0 ; x < 8 ; ++x)
        {
            if(nowWhite) { whiteFieldsList.push_back(FieldMaker(x, y, scene, this->outlinePen4Chessboard, this->whiteBrush)); }
            else { blackFieldsList.push_back(FieldMaker(x, y, scene, this->outlinePen4Chessboard, this->grayBrush)); }
            nowWhite = !nowWhite;
        }
        nowWhite = !nowWhite;
    }
}

void GameScene::SetPawns(QGraphicsScene* scene)
{
    bool skipField = true;
    for(int y = 0 ; y < 3 ; ++y)
    {
        if(skipField)
        {
            for(int x = 1 ; x < 8 ; x=x+2) { blackPawnsList.push_back(PawnMaker(x, y, scene, this->outlinePen4Pawn, this->blackBrush)); }
            skipField = false;
        }
        else
        {
            for(int x = 0 ; x < 8 ; x=x+2) { blackPawnsList.push_back(PawnMaker(x, y, scene, this->outlinePen4Pawn, this->blackBrush)); }
            skipField = true;
        }
    }
    skipField = true;
    for(int y = 5 ; y < 8 ; ++y)
    {
        if(skipField)
        {
            for(int x = 0 ; x < 8 ; x=x+2) { whitePawnsList.push_back(PawnMaker(x, y, scene, this->outlinePen4Pawn, this->whiteBrush)); }
            skipField = false;
        }
        else
        {
            for(int x = 1 ; x < 8 ; x=x+2) { whitePawnsList.push_back(PawnMaker(x, y, scene, this->outlinePen4Pawn, this->whiteBrush)); }
            skipField = true;
        }
    }
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem rectType;
    QGraphicsEllipseItem ellipseType;

    // resetowanie pionkow
    if ( itemAt(event->scenePos(), QTransform::fromScale(1, 1))->type() == ellipseType.type() && this->isPawnSelected )
    {
        if ( itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos() == this->currentlySelectedPawn->pos() && this->whitePlayerMove )
        {
            ResetPawn();
            this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
            this->currentlySelectedPawn = NULL;
            return;
        }
        if ( itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos() == this->currentlySelectedPawn->pos()
             && this->isPawnSelected  && this->whitePlayerMove == false)
        {
            ResetPawn();
            this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
            this->currentlySelectedPawn = NULL;
            return;
        }
    }

    //pionek zaznaczony, klikniecie w pole
    if( itemAt(event->scenePos(), QTransform::fromScale(1, 1))->type() == rectType.type() && this->isPawnSelected )
    {
        for (int i = 0 ; i < blackFieldsList.size() ; i++)
        {
            if (this->whitePlayerMove)
            {
                for (int j = 0 ; j < whitePawnsList.size() ; ++j)
                {
                    if ( this->currentlySelectedPawn->scenePos() == whitePawnsList.at(j)->pawn->scenePos()
                         && whitePawnsList.at(j)->isKing == false)
                    {
                        if ( PosChecker(event, ELEMENTSIZE, -ELEMENTSIZE, i) )
                        {
                            this->currentlySelectedPawn->setPos(itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos());
                            this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
                            this->whitePlayerMove = !this->whitePlayerMove;
                            ResetPawn();
                            return;
                        }
                        if ( PosChecker(event, -ELEMENTSIZE, -ELEMENTSIZE, i) )
                        {
                            this->currentlySelectedPawn->setPos(itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos());
                            this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
                            this->whitePlayerMove = !this->whitePlayerMove;
                            ResetPawn();
                            return;
                        }
                        if ( PosChecker(event, -DISTANCE, -DISTANCE, i) )
                        {
                            Moves(event, -ELEMENTSIZE, -ELEMENTSIZE);
                            return;
                        }
                        if ( PosChecker(event, DISTANCE, -DISTANCE, i) )
                        {
                            Moves(event, ELEMENTSIZE, -ELEMENTSIZE);
                            return;
                        }
                        if ( PosChecker(event, -DISTANCE, DISTANCE, i) )
                        {
                            Moves(event, -ELEMENTSIZE, ELEMENTSIZE);
                            return;
                        }
                        if ( PosChecker(event, DISTANCE, DISTANCE, i) )
                        {
                            Moves(event, ELEMENTSIZE, ELEMENTSIZE);
                            return;
                        }
                    }
                    else if ( this->currentlySelectedPawn->scenePos() == whitePawnsList.at(j)->pawn->scenePos()
                              && whitePawnsList.at(j)->isKing == true)
                    {

                        // tu dla damki ruchy
                    }
                }
            }
            else
            {
                for (int j = 0 ; j < blackPawnsList.size() ; ++j)
                {
                    if ( this->currentlySelectedPawn->scenePos() == blackPawnsList.at(j)->pawn->scenePos()
                         && blackPawnsList.at(j)->isKing == false)
                    {
                        if (PosChecker(event, ELEMENTSIZE, ELEMENTSIZE, i))
                        {
                            this->currentlySelectedPawn->setPos(itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos());
                            this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
                            this->whitePlayerMove = !this->whitePlayerMove;
                            ResetPawn();
                            return;
                        }
                        if ( PosChecker(event, -ELEMENTSIZE, ELEMENTSIZE, i) )
                        {
                            this->currentlySelectedPawn->setPos(itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos());
                            this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
                            this->whitePlayerMove = !this->whitePlayerMove;
                            ResetPawn();
                            return;
                        }
                        if ( PosChecker(event, -DISTANCE, -DISTANCE, i) )
                        {
                            Moves(event, -ELEMENTSIZE, -ELEMENTSIZE);
                            return;
                        }
                        if ( PosChecker(event, DISTANCE, -DISTANCE, i) )
                        {
                            Moves(event, ELEMENTSIZE, -ELEMENTSIZE);
                            return;
                        }
                        if ( PosChecker(event, -DISTANCE, DISTANCE, i) )
                        {
                            Moves(event, -ELEMENTSIZE, ELEMENTSIZE);
                            return;
                        }
                        if ( PosChecker(event, DISTANCE, DISTANCE, i) )
                        {
                            Moves(event, ELEMENTSIZE, ELEMENTSIZE);
                            return;
                        }
                    }
                    else if ( this->currentlySelectedPawn->scenePos() == blackPawnsList.at(j)->pawn->scenePos()
                              && blackPawnsList.at(j)->isKing == true)
                    {

                        // tu dla damki ruchy
                    }
                }
            }
        }
    }

    if ( itemAt(event->scenePos(), QTransform::fromScale(1, 1))->type() == ellipseType.type() && this->isDuringMove == false )
    {
        for (int i = 0 ; i < whitePawnsList.size() ; ++i)
        {
            if ( itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos() == whitePawnsList.at(i)->pawn->pos() && this->whitePlayerMove )
            {
                this->currentlySelectedPawn = whitePawnsList.at(i)->pawn;
                this->currentlySelectedPawn->setPen(this->outlinePen4SelectedPawn);
                ResetPawn();
            }
        }
        if(this->isPawnSelected == false && this->whitePlayerMove == false ) /*<- obvious, but for better clarity*/
            for (int i = 0 ; i < blackPawnsList.size() ; ++i)
            {
                if ( itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos() == blackPawnsList.at(i)->pawn->pos())
                {
                    this->currentlySelectedPawn = blackPawnsList.at(i)->pawn;
                    this->currentlySelectedPawn->setPen(this->outlinePen4SelectedPawn);
                    ResetPawn();
                }
            }
    }
}

bool GameScene::PosChecker(QGraphicsSceneMouseEvent *event, int currentlySPX, int currentlySPY, int i)
{

    bool result = itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos().x() == this->currentlySelectedPawn->pos().x()+currentlySPX
            && itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos().y() == this->currentlySelectedPawn->pos().y()+currentlySPY
            && itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos().x() == blackFieldsList.at(i)->pos().x()
            && itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos().y() == blackFieldsList.at(i)->pos().y();
    return result;
}

bool GameScene::FieldChecker(int numX, int numY)
{
    bool result = false;
    int toGetCenter = 15;
    QGraphicsRectItem rectType;
    for (int i = 0 ; i < blackFieldsList.size() ; ++i)
    {
        if ( this->currentlySelectedPawn->pos().x()+numX >= 0 && this->currentlySelectedPawn->pos().x()+numX <= 420
                && this->currentlySelectedPawn->pos().y()+numY >= 0 && this->currentlySelectedPawn->pos().y()+numY <= 420 )
        {
            if ( (this->currentlySelectedPawn->pos().x()+numX == blackFieldsList.at(i)->pos().x()
                  && this->currentlySelectedPawn->pos().y()+numY == blackFieldsList.at(i)->pos().y())
                  && (itemAt( currentlySelectedPawn->pos().x()+numX+15, currentlySelectedPawn->pos().y()+numY+toGetCenter,
                              QTransform::fromScale(1,1))->type() == rectType.type()))
            { result = true; }
        }
    }
    return result;
}

bool GameScene::PawnChecker(int numX, int numY)
{
    bool result = false;
    if (this->whitePlayerMove)
    {
        for (int j = 0 ; j < blackPawnsList.size() ; ++j)
        {
            if( this->currentlySelectedPawn->pos().x()+numX == blackPawnsList.at(j)->pawn->pos().x()
                    && this->currentlySelectedPawn->pos().y()+numY == blackPawnsList.at(j)->pawn->pos().y() )
            {
                result = true;
            }
        }
    }
    else
    {
        for (int j = 0 ; j < whitePawnsList.size() ; ++j)
            if(this->currentlySelectedPawn->pos().x()+numX == whitePawnsList.at(j)->pawn->pos().x()
                    && this->currentlySelectedPawn->pos().y()+numY == whitePawnsList.at(j)->pawn->pos().y() )
            {
                result = true;
            }
    }
    return result;
}

void GameScene::ResetPawn()
{
    CheckIfKing();
    this->isPawnSelected = !this->isPawnSelected;
    this->isDuringMove = !this->isDuringMove;
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *ev)
{
    static int klikniecie;
    qInfo() << ev->scenePos() << klikniecie++;
}

//
// tu
// stanie sie damka, jezeli bedzie na samej gorze, lub dole odp dla koloru,
// oraz nie bedzie podczas ruchu
void GameScene::CheckIfKing()
{ // podczas zakończenia cyklu ruchów, sprawdź, czy biały pionek jest na kordach y==0, lub czarny
    if(this->whitePlayerMove)
    {
        for(int i = 0 ; i < whitePawnsList.size() ; ++i)
        {
            if(this->currentlySelectedPawn->scenePos().y() == 0
               && this->currentlySelectedPawn->scenePos() == whitePawnsList.at(i)->pawn->scenePos())
            {
                this->whitePawnsList.at(i)->isKing = true;
                this->blackPawnsList.at(i)->pawn->setBrush(this->whiteKingBrush);
                qInfo() << "tworzenie bialej damki";
            }
        }
    }
    else
    {
        for(int i = 0 ; i < blackPawnsList.size() ; ++i)
        {
            if(this->currentlySelectedPawn->scenePos().y() == 7*ELEMENTSIZE
               && this->currentlySelectedPawn->scenePos() == blackPawnsList.at(i)->pawn->scenePos())
            {
                this->blackPawnsList.at(i)->isKing = true;
                this->blackPawnsList.at(i)->pawn->setBrush(this->blackKingBrush);
                qInfo() << "tworzenie czarnej damki";
            }
        }
    }
}

void GameScene::Moves(QGraphicsSceneMouseEvent *event, int numX, int numY)
{
    if (this->whitePlayerMove)
    {
        for (int j = 0 ; j < blackPawnsList.size() ; ++j)
        {
            if ( blackPawnsList.at(j)->pawn->pos().x() == this->currentlySelectedPawn->pos().x()+numX
                 && blackPawnsList.at(j)->pawn->pos().y() == this->currentlySelectedPawn->pos().y()+numY)
            {
                 this->removeItem(blackPawnsList.at(j)->pawn);
                 blackPawnsList.removeAt(j);
                 this->currentlySelectedPawn->setPos(itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos());
                 if ( ( FieldChecker(DISTANCE, DISTANCE) && PawnChecker(ELEMENTSIZE, ELEMENTSIZE) )
                      || ( FieldChecker(DISTANCE, -DISTANCE) && PawnChecker(ELEMENTSIZE, -ELEMENTSIZE) )
                      || ( FieldChecker(-DISTANCE, DISTANCE) && PawnChecker(-ELEMENTSIZE, ELEMENTSIZE) )
                      || ( FieldChecker(-DISTANCE, -DISTANCE) && PawnChecker(-ELEMENTSIZE, -ELEMENTSIZE) ) )
                 {
                     this->isDuringMove = true;
                 }
                 else
                 {
                     this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
                     ResetPawn();
                     this->whitePlayerMove = !this->whitePlayerMove;
                 }
                 return;
            }
        }
    }
    else
    {
        for (int j = 0 ; j < whitePawnsList.size() ; ++j)
        {
            if ( whitePawnsList.at(j)->pawn->pos().x() == this->currentlySelectedPawn->pos().x()+numX
                 && whitePawnsList.at(j)->pawn->pos().y() == this->currentlySelectedPawn->pos().y()+numY)
            {
                 this->removeItem(whitePawnsList.at(j)->pawn);
                 whitePawnsList.removeAt(j);
                 this->currentlySelectedPawn->setPos(itemAt(event->scenePos(), QTransform::fromScale(1, 1))->pos());
                 if ( ( FieldChecker(DISTANCE, DISTANCE) && PawnChecker(ELEMENTSIZE, ELEMENTSIZE) )
                      || ( FieldChecker(DISTANCE, -DISTANCE) && PawnChecker(ELEMENTSIZE, -ELEMENTSIZE) )
                      || ( FieldChecker(-DISTANCE, DISTANCE) && PawnChecker(-ELEMENTSIZE, ELEMENTSIZE) )
                      || ( FieldChecker(-DISTANCE, -DISTANCE) && PawnChecker(-ELEMENTSIZE, -ELEMENTSIZE)) )
                 {
                     this->isDuringMove = true;
                 }
                 else
                 {
                     this->currentlySelectedPawn->setPen(this->outlinePen4Pawn);
                     ResetPawn();
                     this->whitePlayerMove = !this->whitePlayerMove;
                 }
                 return;
            }
        }
    }
}

// dziwny blad przy tworzeniu bialej damki
