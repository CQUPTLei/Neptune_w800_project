# permission\_lite<a name="EN-US_TOPIC_0000001123217533"></a>

-   [Application Permission Management](#section20822104317111)
-   [IPC Authentication](#section156859591110)

## Application Permission Management<a name="section20822104317111"></a>

Application permissions are used to control access to system resources and features related to personal privacy, for example, accessing hardware features of personal devices such as cameras and microphones, and reading and writing media files. The OS protects such data and features through application permission management.

The following table describes fields in a permission.

<a name="table1073153511418"></a>
<table><thead align="left"><tr id="row11107193541417"><th class="cellrowborder" valign="top" width="22.220000000000002%" id="mcps1.1.4.1.1"><p id="p6107535141420"><a name="p6107535141420"></a><a name="p6107535141420"></a>Field</p>
</th>
<th class="cellrowborder" valign="top" width="35.099999999999994%" id="mcps1.1.4.1.2"><p id="p111080352143"><a name="p111080352143"></a><a name="p111080352143"></a>Value</p>
</th>
<th class="cellrowborder" valign="top" width="42.68%" id="mcps1.1.4.1.3"><p id="p161080358141"><a name="p161080358141"></a><a name="p161080358141"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row151081735111418"><td class="cellrowborder" valign="top" width="22.220000000000002%" headers="mcps1.1.4.1.1 "><p id="p1108193521417"><a name="p1108193521417"></a><a name="p1108193521417"></a>name</p>
</td>
<td class="cellrowborder" valign="top" width="35.099999999999994%" headers="mcps1.1.4.1.2 "><p id="p131081435151413"><a name="p131081435151413"></a><a name="p131081435151413"></a>String</p>
</td>
<td class="cellrowborder" valign="top" width="42.68%" headers="mcps1.1.4.1.3 "><p id="p0108235141411"><a name="p0108235141411"></a><a name="p0108235141411"></a>Permission name</p>
</td>
</tr>
<tr id="row19108143516148"><td class="cellrowborder" valign="top" width="22.220000000000002%" headers="mcps1.1.4.1.1 "><p id="p51081355145"><a name="p51081355145"></a><a name="p51081355145"></a>reason</p>
</td>
<td class="cellrowborder" valign="top" width="35.099999999999994%" headers="mcps1.1.4.1.2 "><p id="p01082358147"><a name="p01082358147"></a><a name="p01082358147"></a>Multi-language string ID</p>
</td>
<td class="cellrowborder" valign="top" width="42.68%" headers="mcps1.1.4.1.3 "><p id="p191081235171414"><a name="p191081235171414"></a><a name="p191081235171414"></a>Purpose of requesting the permission</p>
</td>
</tr>
<tr id="row13108123516145"><td class="cellrowborder" valign="top" width="22.220000000000002%" headers="mcps1.1.4.1.1 "><p id="p18109835101415"><a name="p18109835101415"></a><a name="p18109835101415"></a>used-scene{</p>
<p id="p910913358146"><a name="p910913358146"></a><a name="p910913358146"></a>ability,</p>
<p id="p11109235181420"><a name="p11109235181420"></a><a name="p11109235181420"></a>when</p>
<p id="p16109193531417"><a name="p16109193531417"></a><a name="p16109193531417"></a>}</p>
</td>
<td class="cellrowborder" valign="top" width="35.099999999999994%" headers="mcps1.1.4.1.2 "><p id="p4109123511420"><a name="p4109123511420"></a><a name="p4109123511420"></a><strong id="b2227185715217"><a name="b2227185715217"></a><a name="b2227185715217"></a>ability</strong>: string of the component class name</p>
<p id="p19109133531410"><a name="p19109133531410"></a><a name="p19109133531410"></a>when:inuse, always</p>
</td>
<td class="cellrowborder" valign="top" width="42.68%" headers="mcps1.1.4.1.3 "><p id="p31091835151413"><a name="p31091835151413"></a><a name="p31091835151413"></a>Scene where the APIs controlled by this permission are called.</p>
<p id="p93361156407"><a name="p93361156407"></a><a name="p93361156407"></a>This field declares what components can call the APIs controlled by this permission in the specified scene (foreground/background).</p>
</td>
</tr>
</tbody>
</table>

## IPC Authentication<a name="section156859591110"></a>

-   If system services registered with Samgr provide APIs for other processes to access the services through IPC, access control policies must be configured; otherwise, access to the system services will be denied.
-   You can configure access control policies in  **base/security/permission/services/permission\_lite/ipc\_auth/include/policy\_preset.h**.

    1. Define the policies for each feature.

    2. Add the feature policies to the global policy.


Eg.  For example, to configure an access policy for the BMS service, whose service registered with Samgr is  **bundlems**  and whose registered feature is  **BmsFeature**, perform the following operations:

1. Define feature policies. You can configure multiple features and configure multiple access policies for each feature.

**Figure  1**  Example feature policy<a name="fig715515221920"></a>  


![](figures/bms策略举例.png)

There are three types of access policies:

**Figure  2**  Access policy structure<a name="fig1848524515915"></a>  


![](figures/策略类型2.png)

1.    **RANGE**: Processes with UIDs in a specified range can access the BMS service.  **uidMin**  and  **uidMax**  must be specified.

2.    **FIXED**: Processes with specified UIDs can access the BMS service.  **fixedUid**  must be specified, and a maximum of eight UIDs are allowed.

3.    **BUNDLENAME**: A specified application can access the BMS service.  **bundleName**  must be specified.

2. Add the defined feature policies to the global policy. You need to configure the number of features.

**Figure  3**  Registering a feature policy<a name="fig1181753551014"></a>  


![](figures/全局策略2.png)

UID allocation rules:

1. Init process: 0

2. appspawn process: 1

3. Shell process: 2

4. Other built-in system services: less than or equal to 99

5. System applications \(such as settings, home screen, and camera\): 100–999

6. Preset applications: 1000–9999

7. Common third-party applications: 10000 to  **INT\_MAX**

