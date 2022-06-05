#pragma once

#include <utils/math/Transform4x4f.h>
#include <utils/math/Vector2f.h>
#include <utils/math/Vector3f.h>
#include <themes/Properties.h>
#include "IComponent.h"

#include <functional>
#include <vector>
#include <string>

class WindowManager;
class Animation;
class AnimationController;
class ThemeData;
class Font;
class InputCompactEvent;
class Help;
class HelpStyle;
class ThemeData;

class Component: public IComponent
{
  public:
    explicit Component(WindowManager& window);
    virtual ~Component();

    /*!
     * @brief Called from SDL in text input mode
     * @param text Latest text
     * @param cursor position
     */
    virtual void textInput(const char* text);

    /*!
     * @brief Notification of an input event
     * @param event Compact event
     * @return Implementation must return true if it consumed the event.
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
     * @brief Called once per frame. Override to implement your own drawings.
     * Call your base::Update() to ensure animation and childrens are updated properly
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    void Update(int deltaTime) override;

    /*!
     * @brief Called once per frame, after Update
     * Implement your own Render() to draw your own compponents or over-drawings
     * First, execute: 	Transform4x4f trans = (parentTrans * getTransform()).round();
	   *                  Renderer::setMatrix(trans);
     * Then draw your components
     * Finally, call your base.Render(trans) to draw animations and childrens
     * @param parentTrans Transformation
     */
    void Render(const Transform4x4f& parentTrans) override;

    inline bool isThemeDisabled() const { return mThemeDisabled; }
    inline void setThemeDisabled(bool disabled) { mThemeDisabled = disabled; }

    const Vector3f& getPosition() const { return mPosition; }
    void setNormalisedPosition(float x, float y, float z = 0.0f);
    inline void setPosition(const Vector3f& offset) { setPosition(offset.x(), offset.y(), offset.z()); }
    void setPosition(float x, float y, float z = 0.0f);
    virtual void onPositionChanged() {};

    //Sets the origin as a percentage of this image (e.g. (0, 0) is top left, (0.5, 0.5) is the center)
    const Vector2f& getOrigin() const { return mOrigin; }
    void setOrigin(float originX, float originY);
    inline void setOrigin(const Vector2f& origin) { setOrigin(origin.x(), origin.y()); }
    virtual void onOriginChanged() {};

    //Sets the rotation origin as a percentage of this image (e.g. (0, 0) is top left, (0.5, 0.5) is the center)
    const Vector2f& getRotationOrigin() const { return mRotationOrigin; }
    void setRotationOrigin(float originX, float originY);
    inline void setRotationOrigin(const Vector2f& origin) { setRotationOrigin(origin.x(), origin.y()); }

    const Vector2f& getSize() const {	return mSize; }
    inline void setSize(const Vector2f& size) { setSize(size.x(), size.y()); }
    void setSize(float w, float h);

    /*!
     * @brief Called when the component size is changed
     */
    virtual void onSizeChanged() {};

    float getRotation() const { return mRotation; }
    void setRotation(float rotation) { mRotation = rotation; }
    inline void setRotationDegrees(float rotation) { setRotation(rotation * (float)3.14159265359 / 180.0f); }

    inline float getScale() const { return mScale; }
    inline void setScale(float scale) { mScale = scale; }

    inline float getZIndex() const { return mZIndex; }
    inline void setZIndex(float zIndex) { mZIndex = zIndex; mThemeDisabled = false; }

    inline float getDefaultZIndex() const { return mDefaultZIndex; }
    inline void setDefaultZIndex(float zIndex) { mDefaultZIndex = zIndex; }

    // Returns the center point of the image (takes origin into account).
    Vector2f getCenter() const;

    void setParent(Component* parent) { mParent = parent; }
    Component* getParent() const { return mParent; }

    void addChild(Component* cmp);
    void removeChild(Component* cmp);
    void clearChildren();
    void sortChildren();
    unsigned int getChildCount() const;
    Component* getChild(unsigned int i) const;

    // animation will be automatically deleted when it completes or is stopped.
    bool isAnimationPlaying(unsigned char slot) const;
    bool isAnimationReversed(unsigned char slot) const;
    int getAnimationTime(unsigned char slot) const;
    void setAnimation(Animation* animation, int delay = 0, const std::function<void()>& finishedCallback = nullptr, bool reverse = false, unsigned char slot = 0);
    bool stopAnimation(unsigned char slot);
    bool cancelAnimation(unsigned char slot); // Like stopAnimation, but doesn't call finishedCallback - only removes the animation, leaving things in their current state.  Returns true if successful (an animation was in this slot).
    bool finishAnimation(unsigned char slot); // Calls update(1.f) and finishedCallback, then deletes the animation - basically skips to the end.  Returns true if successful (an animation was in this slot).
    bool advanceAnimation(unsigned char slot, unsigned int time); // Returns true if successful (an animation was in this slot).
    void stopAllAnimations();
    void cancelAllAnimations();

    virtual unsigned char getOpacity() const { return mOpacity; }
    virtual void setOpacity(unsigned char opacity);

    const Transform4x4f& getTransform();

    virtual std::string getValue() const { return ""; }
    virtual void setValue(const std::string& ) { }
    virtual void setColor(unsigned int ) { }
    virtual unsigned int getOriginColor() {	return 0; }

    virtual void onFocusGained() {};
    virtual void onFocusLost() {};

    virtual void onShow();
    virtual void onHide();


    // Default implementation just handles <pos> and <size> tags as normalized float pairs.
    // You probably want to keep this behavior for any derived classes as well as add your own.
    virtual void applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties);

    /*!
     * @brief Fill help list
     * @param help Help list to fill
     * @return True if the method filled the list, false otherwise
     */
    virtual bool getHelpPrompts(Help& help) { (void)help; return false; };

    // Called whenever help prompts change.
    void updateHelpPrompts();

    virtual void ApplyHelpStyle();

    virtual inline void setScrollDir(int dir) { (void)dir; }

    // Returns true if the component is busy doing background processing (e.g. HTTP downloads)
    bool isProcessing() const { return mIsProcessing; }

    const static unsigned char MAX_ANIMATIONS = 4;

  private:
    Transform4x4f mTransform; //Don't access this directly! Use getTransform()!
    AnimationController* mAnimationMap[MAX_ANIMATIONS];
    // mChildren has been moved from value to reference, because most component instances do not have any child.
    // Doing this saves 20 octets by instance
    std::vector<Component*>* mChildren;

  protected:
    static Help& HelpItems();
    static HelpStyle& HelpItemStyle();

    void renderChildren(const Transform4x4f& transform) const;
    void updateSelf(int deltaTime); // updates animations
    void updateChildren(int deltaTime); // updates animations

    Vector2f denormalise(float x, float y);
    Vector2f denormalise(const Vector2f& value);

    WindowManager& mWindow;
    Component* mParent;

    Vector3f mPosition;
    Vector2f mOrigin;
    Vector2f mRotationOrigin;
    Vector2f mSize;

    float mRotation;
    float mScale;

    float mDefaultZIndex;
    float mZIndex;

    unsigned char mOpacity;
    bool mIsProcessing;
    bool mThemeDisabled;
};
