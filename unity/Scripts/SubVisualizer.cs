// Place this script on any GameObject in your scene.
// It subscribes to the ROS2 depth topic and moves the object's Y position
// to match the incoming value.

using UnityEngine;
using Unity.Robotics.ROSTCPConnector;
using RosMessageTypes.Std;

public class SubVisualizer : MonoBehaviour
{
    [SerializeField] private string topicName = "/state/depth";

    // Cached so the callback (called on a background thread) can hand the
    // value to Update() on the main thread without touching the Transform directly.
    private float pendingDepth_;
    private bool  hasNewDepth_ = false;

    void Start()
    {
        ROSConnection.GetOrCreateInstance().Subscribe<Float64Msg>(topicName, OnDepthReceived);
    }

    void Update()
    {
        // Transform writes must happen on Unity's main thread.
        if (!hasNewDepth_) return;

        Vector3 pos = transform.position;
        pos.y = pendingDepth_;
        transform.position = pos;
        hasNewDepth_ = false;
    }

    // Called by ROSConnection on a background thread whenever a message arrives.
    private void OnDepthReceived(Float64Msg msg)
    {
        pendingDepth_ = (float)msg.data;
        hasNewDepth_  = true;
    }
}
